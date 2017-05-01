#include "HoughFeatureExtractor.h"
#include "boost/multi_array.hpp"
#include "MathHelper.h"
#include "DebugHelper.h"

HoughFeatureExtractor::HoughFeatureExtractor()
{
}


HoughFeatureExtractor::~HoughFeatureExtractor()
{
}

bool IsLocalMaximum(
	const boost::multi_array<pair<double, vector<Point>>, 2>& lineParametersSpace,
	int ro,
	int fi,
	int roBuckets,
	int fiBuckets,
	int windowHalfSize = 2
)
{
	auto accumulatedLines = lineParametersSpace[ro][fi];

	for (int dRo = -windowHalfSize; dRo <= windowHalfSize; dRo++)
	{
		for (int dFi = -windowHalfSize; dFi <= windowHalfSize; dFi++)
		{
			if (dFi == 0 && dRo == 0
				|| dRo + ro<0
				|| dRo + ro>roBuckets-1
				|| dFi + fi<0
				|| dFi + fi>fiBuckets-1)
			{
				continue;
			}
			if (accumulatedLines.first < lineParametersSpace[ro + dRo][fi + dFi].first)
			{
				return false;
			}
		}
	}
	return true;
}

double TransformCoordinates(double x, double max, double min, double totalBuckets)
{
	double step = (max - min)/totalBuckets;
	return (x - min) / step;
}

double TransformCoordinates(double x, double min, double step)
{
	return (x - min) / step;	
}

double TransformCoordinatesBack(double x,double min, double step)
{
	return (x+0.5) * step + min;
}

double epsilon = 0.01;
vector<vector<Point>> HoughFeatureExtractor::FindLines(
	const Matrix2D & edges,
	const Matrix2D & magnitude,
	const Matrix2D & directions,
	const LineSpaceSettings& settings
)
{
	double maxAngle = 2*M_PI;
	double minAngle = 0;
	int roCells = (settings.roMax - settings.roMin) / settings.roStep + 1;
	int angleCells = (maxAngle - minAngle) / settings.angleStep;
	//fi,ro
	boost::multi_array<pair<double, vector<Point>>, 2> lineParametersSpace(
		boost::extents
		[roCells]
	[angleCells]);
	int height = edges.Height();
	int width = edges.Width();
	for (int y = 0; y < edges.Height(); y++)
	{
		for (int x = 0; x < edges.Width(); x++)
		{
			if (edges.At(x, y) > epsilon)
			{
				int centeredX = -(x - width / 2.);
				int centeredY = -(y - height / 2.);
				double angle = directions.At(x, y);
				
				double teta = atan2(centeredY, centeredX) - angle;
				double ro = hypot(centeredX, centeredY)*cos(teta);
				double dx = cos(angle);
				double dy = sin(angle);
				double crossX = (sqr(dx)*centeredX + dy*centeredY*dx) / (sqr(dx)+sqr(dy));
				double crossY = crossX * dy / dx;
				double ro2 = hypot(crossX,crossY);
				angle = atan2(crossY, crossX);
				double anglePositive = fmod(angle + 2*M_PI, 2*M_PI);
				double angleNormalized = anglePositive / settings.angleStep;
				int fiBucket = int(angleNormalized + 0.5) - 1;
				//assert(abs(ro - ro2) < 0.1);
				double roNormalized =
					TransformCoordinates(
						ro, 						 
						settings.roMax,
						settings.roMin,
						roCells);
				int roBucket = int(roNormalized + 0.5) - 1;
				for (int dRo = 0;dRo < 2;dRo++)
				{
					int currentRo = roBucket + dRo;
					double roCenter = currentRo + 0.5;
					double wRo = abs(roNormalized - roCenter);
					for (int dFi = 0; dFi < 2; dFi++)
					{
						int currentFi = (fiBucket + dFi + angleCells) % angleCells;
						double fiCenter = fiBucket + dFi + 0.5;
						double wFi = abs(angleNormalized - fiCenter);
						assert(wRo < 1.001 && wFi < 1.001);
						lineParametersSpace[currentRo][currentFi].first += wRo*wFi;
						lineParametersSpace[currentRo][currentFi].second.push_back(Point(x, y));
					}
				}
			}
		}
	}
	vector<vector<Point>> result;
	for (int ro = 0; ro < roCells; ro++)
	{
		for (int fi = 0; fi < angleCells; fi++)
		{
			auto accumulatedLines = lineParametersSpace[ro][fi];
			if (accumulatedLines.first > 30 && 
				IsLocalMaximum(lineParametersSpace,ro,fi,roCells,angleCells))
			{
				result.push_back(accumulatedLines.second);
			}
		}

	}
	return result;
}

bool IsLocalMaximum(
	const boost::multi_array<float, 3>& circleParametersSpace,
	int x,
	int y,
	int r,
	int xBuckets,
	int yBuckets,
	int rBuckets,
	int windowHalfSize = 2
)
{
	auto currentWeight = circleParametersSpace[y][x][r];

	for (int dy = -windowHalfSize; dy <= windowHalfSize; dy++)
	{
		for (int dx = -windowHalfSize; dx <= windowHalfSize; dx++)
		{
			for (int dr = -windowHalfSize; dr <= windowHalfSize; dr++)
			{
				if (dx == 0 && dy == 0 && dr==0
					|| dx + x<0
					|| dx + x>xBuckets - 1
					|| dy + y<0
					|| dy + y>yBuckets - 1
					|| dr + r<0
					|| dr + r>rBuckets - 1
					)
				{
					continue;
				}
				if (currentWeight < circleParametersSpace[y+dy][x+dx][r+dr])
				{
					return false;
				}
			}
		}
	}
	return true;
}


vector<CircleDescriptor> HoughFeatureExtractor::FindCircles(
	const Matrix2D & edges, 
	const Matrix2D & magnitude, 
	const Matrix2D & directions,
	const CircleSpaceSettings & settings)
{
	int width = edges.Width();
	int height = edges.Height();
	double angle;
	double directionX;
	double directionY;
	int centerXCells = (settings.xMax - settings.xMin) / settings.centerStep + 1;
	int centerYCells = (settings.yMax - settings.yMin) / settings.centerStep + 1;
	int rCells = (settings.rMax - settings.rMin) / settings.rStep + 1;
	boost::multi_array<float, 3> circleParametersSpace(boost::extents[centerYCells][centerXCells][rCells]);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (edges.At(x, y) > 0)
			{
				angle = directions.At(x, y);
				directionX = cos(angle);
				directionY = sin(angle);
				for (int r = settings.rMin; r < settings.rMax; r++)
				{
					double normalizedR = TransformCoordinates(
						r, 
						settings.rMax, 
						settings.rMin, 
						rCells);
					double normalizedX1 = TransformCoordinates(
						x + directionX*r, 
						settings.xMax, 
						settings.xMin, 
						centerXCells);
					double normalizedY1 = TransformCoordinates(
						y + directionY*r, 
						settings.yMax, 
						settings.yMin, 
						centerYCells);
					double normalizedX2 = TransformCoordinates(
						x - directionX*r, 
						settings.xMax, 
						settings.xMin, 
						centerXCells);
					double normalizedY2 = TransformCoordinates(
						y - directionY*r, 
						settings.yMax, 
						settings.yMin, 
						centerYCells);

					int xBucket1 = int(normalizedX1 + 0.5) - 1;
					int yBucket1 = int(normalizedY1 + 0.5) - 1;
					int xBucket2 = int(normalizedX2 + 0.5) - 1;
					int yBucket2 = int(normalizedY2 + 0.5) - 1;
					int rBucket = int(normalizedR + 0.5) - 1;
					for (int dy = 0;dy < 2;dy++)
					{
						int currentY1 = yBucket1 + dy;
						double yCenter1 = currentY1 + 0.5;
						double wy1 = abs(normalizedY1 - yCenter1);

						int currentY2 = yBucket2 + dy;
						double yCenter2 = currentY2 + 0.5;
						double wy2 = abs(normalizedY2 - yCenter2);

						for (int dx = 0;dx < 2;dx++)
						{
							int currentX1 = xBucket1 + dx;
							double xCenter1 = currentX1 + 0.5;
							double wx1 = abs(normalizedX1 - xCenter1);

							int currentX2 = xBucket2 + dx;
							double xCenter2 = currentX2 + 0.5;
							double wx2 = abs(normalizedX2 - xCenter2);
							
							for (int dr = 0;dr < 2;dr++) 
							{
								int currentR = rBucket + dr;
								if (currentR < 0 || currentR>rCells - 1)
								{
									continue;
								}
								double rCenter = currentR + 0.5;
								double wr = abs(normalizedR - rCenter);
								assert(wx1 < 1.01);
								assert(wx2 < 1.01);
								assert(wy1 < 1.01);
								assert(wy2 < 1.01);
								assert(wr < 1.01);

								circleParametersSpace[currentY1][currentX1][currentR] += wx1*wy1*wr;
								circleParametersSpace[currentY2][currentX2][currentR] += wx2*wy2*wr;
							}
						}
					}
				}
			}
		}
	}
	vector<CircleDescriptor> result;
	for (int y = 0; y < centerYCells; y++)
	{
		for (int x = 0; x < centerXCells; x++)
		{
			for (int r = 0; r < rCells; r++)
			{		
				double weight = circleParametersSpace[y][x][r];
				if (weight/log2(r+2)>14 && IsLocalMaximum(circleParametersSpace,x,y,r,centerXCells,centerYCells,rCells))
				{
					double transformedR = r*settings.rStep + settings.rMin;
					double transformedX = (x + 0.5)*settings.centerStep + settings.xMin;
					double transformedY = (y + 0.5)*settings.centerStep + settings.yMin;
					result.push_back(CircleDescriptor(transformedX, transformedY,transformedR));
				}
			}
		}
	}
	return result;
}

bool PointBelongsToEllipse(const Point& point,const EllipseDescriptor& ellipse) 
{
	return abs(
		sqr((point.x - ellipse.x)*cos(ellipse.fi) + (point.y - ellipse.y) * sin(ellipse.fi))
		/ sqr(ellipse.a) +
		sqr((point.x - ellipse.x)*sin(ellipse.fi) - (point.y - ellipse.y)*cos(ellipse.fi))
		/ sqr(ellipse.b) - 1) < 1;
}

bool HasAnyEdge(const Matrix2D& edges, int x, int y, int edgeStep)
{
	int counter =0;
	for (int dy = -edgeStep/2; dy < edgeStep/2+1; dy++)
	{
		for (int dx = -edgeStep/2; dx < edgeStep/2+1; dx++)
		{
			if (edges.GetIntensity(x + dx, y + dy, BorderMode::zero) > 1 - epsilon)
			{
				return true;
			}
		}
	}
	return false;
	//return false;
}

bool IsLocalMaximum(
	const boost::multi_array<float, 5>& ellipseParameterSpace,
	int x,
	int y,
	int a,
	int b,
	int fi,
	int xBuckets,
	int yBuckets,
	int aBuckets,
	int bBuckets,
	int fiBuckets,
	int windowHalfSize = 2
	)
{
	auto currentWeight = ellipseParameterSpace[x][y][a][b][fi];

	for (int dx = -windowHalfSize; dx <= windowHalfSize; dx++)
	{
		for (int dy = -windowHalfSize; dy <= windowHalfSize; dy++)
		{
			for (int da = -windowHalfSize; da <= windowHalfSize; da++)
			{
				for (int db = -windowHalfSize; db <= windowHalfSize; db++)
				{
					for (int dfi = -windowHalfSize; dfi <= windowHalfSize; dfi++)
					{
						if (dx == 0 && dy == 0 && da == 0 && db ==0 && dfi ==0 
							|| dx + x<0
							|| dx + x>xBuckets - 1
							|| dy + y<0
							|| dy + y>yBuckets - 1
							|| da + a<0
							|| da + a>aBuckets - 1
							|| db + b<0
							|| db + b>bBuckets - 1
							)
						{
							continue;
						}
						if (currentWeight < ellipseParameterSpace
							[x + dx]
							[y + dy]
							[a + da]
							[b+db]
							[(fi+dfi+fiBuckets)%fiBuckets])
						{
							return false;
						}
					}
				}
			}
		}
	}
	return true;
}

double CalculateSupportRatio(const Matrix2D& edges, EllipseDescriptor ellipse)
{
	int supported = 0;
	int notSupported = 0;
	for (int y = ellipse.y-ellipse.a; y < ellipse.y+ellipse.a+1; y++)
	{
		for (int x = ellipse.x-ellipse.a; x < ellipse.x+ellipse.a+1; x++)
		{
			if (PointBelongsToEllipse(Point(x, y), ellipse)) 
			{
				if (HasAnyEdge(edges, x, y, 4)) 
				{
					supported++;
				}
				else
				{
					notSupported++;
				}
			}
		}
	}
	return 1.*supported / (supported+notSupported);
}

vector<EllipseDescriptor> HoughFeatureExtractor::FindEllipses(
	const Matrix2D & edges, 
	const Matrix2D & magnitude, 
	const Matrix2D & directions,
	const EllipseSpaceSettings& settings)
{
	int height = edges.Height();
	int width = edges.Width();
	vector<EllipseDescriptor> result;	
	vector<Point> edgesPoints;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (edges.At(x,y) > epsilon)
			{
				edgesPoints.push_back(Point(x, y));
			}
		}
	}

	int xCells = (settings.xMax - settings.xMin) / settings.centerStep + 1;
	int yCells = (settings.yMax - settings.yMin) / settings.centerStep + 1;
	int aCells = (settings.rMax - settings.rMin) / settings.rStep + 1;
	int bCells = (settings.rMax - settings.rMin) / settings.rStep + 1;
	int fiCells = (settings.fiMax - settings.fiMin) / settings.fiStep;
	boost::multi_array<float, 5> ellipseParametersSpace(boost::extents[xCells][yCells][aCells][bCells][fiCells]);
	//vector<int> bAccumulator((rMax - rMin) / rStep);

	for (int p1Index = 0; p1Index < edgesPoints.size(); p1Index++)
	{
		double x1 = edgesPoints[p1Index].x;
		double y1 = edgesPoints[p1Index].y;
		for (int p2Index = p1Index + 1; p2Index < edgesPoints.size(); p2Index++)
		{
			double x2 = edgesPoints[p2Index].x;
			double y2 = edgesPoints[p2Index].y;
			double a = hypot(x2 - x1, y2 - y1) / 2;
			if (!(a > settings.rMin && a < settings.rMax))
			{
				continue;
			}
			double x0 = (x1 + x2) / 2.;
			double y0 = (y1 + y2) / 2.;

			double slope = atan2(y2 - y1, x2 - x1);
			double point1Slope = atan2(sin(directions.At(x1, y1)), cos(directions.At(x1, y1)));
			if (abs(fmod(slope + M_PI, M_PI) - fmod(point1Slope + M_PI, M_PI)) > M_PI / 18)
				continue;

			double point2Slope = atan2(sin(directions.At(x2, y2)), cos(directions.At(x2, y2)));
			if (abs(fmod(slope + M_PI, M_PI) - fmod(point2Slope + M_PI, M_PI)) > M_PI / 18) 
				continue;
			
			for (int p3Index = 0; p3Index < edgesPoints.size(); p3Index++)
			{
				double x3 = edgesPoints[p3Index].x;
				double y3 = edgesPoints[p3Index].y;
				if (p3Index == p2Index || p3Index == p1Index)
				{
					continue;
				}
				double d = hypot(x3 - x0, y3 - y0);
				
				if (d < a && d>settings.rMin)
				{
					double cosTau = ((x3 - x0)*(x2 - x1) + (y3 - y0)*(y2 - y1)) / (2 * a*d);
					double b = sqrt(sqr(a)*sqr(d)*(1 - sqr(cosTau)) / (sqr(a) - sqr(d)*sqr(cosTau)));
					if (b < a && b < settings.rMax && b>settings.rMin)
					{
						double normalizedX = TransformCoordinates(x0, settings.xMin, settings.centerStep);
						double normalizedY = TransformCoordinates(y0, settings.yMin, settings.centerStep);
						double normalizedA = TransformCoordinates(a, settings.rMin, settings.rStep);
						double normalizedB = TransformCoordinates(b, settings.rMin, settings.rStep);;
						double normalizedFi = TransformCoordinates(slope, settings.fiMax, settings.fiMin, fiCells);;;

						int xBucket = int(normalizedX + 0.5) - 1;
						int yBucket = int(normalizedY + 0.5) - 1;
						int aBucket = int(normalizedA + 0.5) - 1;
						int bBucket = int(normalizedB + 0.5) - 1;
						int fiBucket = int(normalizedFi + 0.5) - 1;
						for (int dx = 0;dx < 2;dx++)
						{
							int currentX = xBucket + dx;
							if (currentX < 0) continue;
							double xCenter = currentX + 0.5;
							double wx = abs(normalizedX - xCenter);
							for (int dy = 0;dy < 2;dy++)
							{
								int currentY = yBucket + dy;
								if (currentY < 0) continue;
								double yCenter = currentY + 0.5;
								double wy = abs(normalizedY - yCenter);
								for (int da = 0;da < 2;da++)
								{
									int currentA = aBucket + da;
									if (currentA < 0) continue;
									double aCenter = currentA + 0.5;
									double wa = abs(normalizedA - aCenter);
									for (int db = 0;db < 2;db++)
									{
										int currentB = bBucket + db;
										if (currentB < 0) continue;
										double bCenter = currentB + 0.5;
										double wb = abs(normalizedB - bCenter);
										for (int dfi = 0;dfi < 2;dfi++)
										{
											int currentFi = fiBucket + dfi;
											double fiCenter = currentFi + 0.5;
											currentFi = (currentFi + fiCells) % fiCells;
											double wfi = abs(normalizedFi - fiCenter);
											assert(wx < 1.01);
											assert(wy < 1.01);
											assert(wa < 1.01);
											assert(wfi < 1.01);
											ellipseParametersSpace
												[currentX]
											[currentY]
											[currentA]
											[currentB]
											[currentFi] += wx*wy*wa*wb*wfi;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	
		
	for (int x0 = 0; x0 < xCells; x0++)
	{
		for (int y0 = 0; y0 < yCells; y0++)
		{
			for (int a = 0; a < aCells; a++)
			{
				for (int b = 0; b < bCells; b++)
				{
					for (int fi = 0;fi < fiCells;fi++)
					{
						auto& weight = ellipseParametersSpace[x0][y0][a][b][fi];
						
						if (weight / (hypot(a, b)+epsilon) > settings.threshold &&
							IsLocalMaximum(
								ellipseParametersSpace,
								x0,
								y0,
								a,
								b,
								fi,
								xCells,
								yCells,
								aCells,
								bCells,
								fiCells)
							)
							
						{
							
							double transformedX0 = TransformCoordinatesBack(x0, settings.xMin, settings.centerStep);
							double transformedY0 = TransformCoordinatesBack(y0, settings.yMin, settings.centerStep);
							double transformedA = TransformCoordinatesBack(a, settings.rMin, settings.rStep);
							double transformedB = TransformCoordinatesBack(b, settings.rMin, settings.rStep);
							double transformedFi = TransformCoordinatesBack(fi, settings.fiMin, settings.fiStep);
							auto& ellipse = EllipseDescriptor(transformedX0, transformedY0, transformedA, transformedB, transformedFi);
							if (CalculateSupportRatio(edges, ellipse) > 0.4)
							{
								result.push_back(ellipse);
							}
						}
					}
				}
			}
		}
	}
			

		


	
	return result;

}




TransformationMetaInfo RecalcuteRelativeCoordinates(TransformationMetaInfo& metaInfo,Size objectSize) 
{
	double dx = (objectSize.Center().x - metaInfo.x);
	double dy = (objectSize.Center().y - metaInfo.y);
	double alpha = atan2(dy, dx);
	double rotation = -alpha + metaInfo.rotation;
	double scale = metaInfo.scale;
	return TransformationMetaInfo(dx,dy,scale,rotation);
}

TransformationMetaInfo TransformSpaceBack(int x, int y, double r, double s, const DescriptorDimentionSettings& cfg )
{
	return TransformationMetaInfo(
		((x) * cfg.stepX) + cfg.minX,
		((y)* cfg.stepY) + cfg.minY,		
		pow(2, s+0.5) * cfg.scaleMin,
		(r) * cfg.rotationStep
	);
}

void Vote(
	boost::multi_array<pair<double, vector<int>>, 4>& transformSpace,
	double normalizedX, 
	double normalizedY, 
	double normalizedRotation,
	double normalizedScale,
	int rCells,
	int index)
{
	int scaleBucket = int(normalizedScale + 0.5) - 1;
	int xBucket = int(normalizedX + 0.5) - 1;
	int yBucket = int(normalizedY + 0.5) - 1;
	int rotationBucket = int(normalizedRotation + 0.5) - 1;
	for (int dx = 0; dx < 2; dx++)
	{
		int currentX = xBucket + dx;
		double xCenter = currentX + 0.5;
		double wx = abs(normalizedX - xCenter);
		for (int dy = 0; dy < 2; dy++)
		{
			int currentY = yBucket + dy;
			double yCenter = currentY + 0.5;
			double wy = abs(normalizedY - yCenter);
			for (int dr = 0; dr < 2; dr++)
			{
				int currentR = rotationBucket + dr;
				double rCenter = currentR + 0.5;
				double wr = abs(normalizedRotation - rCenter);
				currentR = (currentR + rCells) % rCells;
				for (int ds = 0; ds < 2; ds++)
				{
					int currentS = scaleBucket + ds;
					double sCenter = currentS + 0.5;
					double ws = abs(normalizedScale - sCenter);
					transformSpace[currentX][currentY][currentR][currentS]
						.first += wx*wy*wr*ws;
					transformSpace[currentX][currentY][currentR][currentS]
						.second.push_back(index);
				}
			}
		}
	}
}

TransformationMetaInfo PrepareVoteBins(
	const pair<Descriptor,Descriptor>& match,
	Size objectSize,
	const DescriptorDimentionSettings& settings
) 
{
	auto& objectDescriptor = match.first;

	auto& objectMetaInfo = objectDescriptor.MetaInfo();
	auto objectDescriptorInfo = RecalcuteRelativeCoordinates(objectMetaInfo, objectSize);
	auto& sceneDescriptorMeta = match.second.MetaInfo();
	double sceneObjectScale = sceneDescriptorMeta.scale / objectDescriptorInfo.scale;
	double sceneObjectCenterAngle = sceneDescriptorMeta.rotation - objectDescriptorInfo.rotation;
	auto vectorToCenterLenght =
		sceneDescriptorMeta.scale / objectDescriptorInfo.scale
		*
		hypot(objectDescriptorInfo.x, objectDescriptorInfo.y);
	auto dx = vectorToCenterLenght*cos(sceneObjectCenterAngle);
	auto dy = vectorToCenterLenght*sin(sceneObjectCenterAngle);
	double sceneObjectCenterX = sceneDescriptorMeta.x + dx;
	double sceneObjectCenterY = sceneDescriptorMeta.y + dy;

	double normalizedScale = log2(sceneObjectScale / settings.scaleMin);
	double normalizedX = (sceneObjectCenterX - settings.minX) / settings.stepX;
	double normalizedY = (sceneObjectCenterY - settings.minY) / settings.stepY;
	double normalizedRotation = (sceneDescriptorMeta.rotation - objectMetaInfo.rotation)
		/ settings.rotationStep;
	return TransformationMetaInfo(normalizedX, normalizedY, normalizedScale, normalizedRotation);
}

pair<TransformationMetaInfo, vector<int>> ChooseBestCandidate(
	boost::multi_array<pair<double, vector<int>>, 4>& transformSpace,
	int xCells,
	int yCells,
	int rCells,
	int sCells,
	const DescriptorDimentionSettings& settings) 
{
	double threshold = 4;
	vector<int> maxInliers;
	double maxWeight = 0;
	TransformationMetaInfo bestParameters;
	for (int x = 0;x<xCells; x++)
		for (int y = 0; y < yCells; y++)
			for (int r = 0; r < rCells; r++)
				for (int s = 0; s < sCells; s++)
				{
					if (transformSpace[x][y][r][s].first < threshold
						&& transformSpace[x][y][r][s].second.size() < 3)
					{
						continue;
					}
					if (maxWeight < transformSpace[x][y][r][s].first)
					{
						bestParameters = TransformSpaceBack(x, y, r, s, settings);
						maxWeight = transformSpace[x][y][r][s].first;
						maxInliers = transformSpace[x][y][r][s].second;
					}
				}
	return{ bestParameters,maxInliers };
}

pair<TransformationMetaInfo,vector<int>> HoughFeatureExtractor::FindObjectPose(
	Size objectSize,
	const vector<pair<Descriptor, Descriptor>>& matches,
	const DescriptorDimentionSettings& settings
)
{
	int sCells = log2(settings.scaleMax) + log2(1/settings.scaleMin) + 1;
	int xCells = (settings.maxX - settings.minX) / settings.stepX + 1;
	int yCells = (settings.maxY - settings.minY) / settings.stepY + 1;
	int rCells = 2 * M_PI / settings.rotationStep;
	boost::multi_array<pair<double, vector<int>>, 4> transformSpace(boost::extents
		[xCells][yCells][rCells][sCells]);

	for (int i = 0; i < matches.size();i++)
	{
		auto& match = matches[i];
		
		auto binsInfo = PrepareVoteBins(match,objectSize ,settings);
		Vote(
			transformSpace, 
			binsInfo.x,
			binsInfo.y,
			binsInfo.rotation,
			binsInfo.scale,
			rCells,
			i);
	}
	return ChooseBestCandidate(
		transformSpace,
		xCells,
		yCells,
		rCells,
		sCells,
		settings);
}
