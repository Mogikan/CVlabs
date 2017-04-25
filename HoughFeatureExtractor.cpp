#include "HoughFeatureExtractor.h"
#include "boost/multi_array.hpp"
#include "MathHelper.h"

HoughFeatureExtractor::HoughFeatureExtractor()
{
}


HoughFeatureExtractor::~HoughFeatureExtractor()
{
}

vector<vector<Point>> HoughFeatureExtractor::FindLines(const Matrix2D & edges, const Matrix2D & magnitude, const Matrix2D & directions,int roStep,int angleStep)
{
	int roMax = hypot(magnitude.Width(), magnitude.Height());
	int maxAngle = 360;
	//fi,ro
	boost::multi_array<vector<Point>, 2> lineParametersSpace(boost::extents[roMax/roStep*2+1][maxAngle/angleStep+1]);

	
	for (int y = 0; y < edges.Height(); y++)
	{
		for (int x = 0; x < edges.Width(); x++)
		{
			if (edges.At(x, y) > 0)
			{
				double angleRadians = directions.At(x, y);
				double angleRadNormalized = fmod(angleRadians+2 * M_PI,2 * M_PI);
				int fi = int(angleRadNormalized/M_PI*180)/angleStep;
				int height = edges.Height();
				int width = edges.Width();
				int ro = (((x-width/2.)*cos(angleRadians)+(y-height/2.)*sin(angleRadians))+roMax/2)/roStep;
				lineParametersSpace[ro][int(fi)].push_back(Point(x, y));
			}
		}
	}
	vector<vector<Point>> result;
	for (int ro = 0; ro < roMax*2/roStep; ro++)
	{
		for (int fi = 0; fi < maxAngle/angleStep; fi++)
		{
			auto accumulatedLines = lineParametersSpace[ro][fi];
			if (accumulatedLines.size() > 20
				)
			{				
				result.push_back(accumulatedLines);
			}
		}
	}
	return result;
}

vector<vector<Point>> HoughFeatureExtractor::FindCircles(
	const Matrix2D & edges, 
	const Matrix2D & magnitude, 
	const Matrix2D & directions,
	int rMin, 
	int rMax,
	int rStep,
	int centerStep)
{
	int width = edges.Width();
	int height = edges.Height();
	double angle;
	double dx;
	double dy;
	if (rMax > width / 2.)
	{
		rMax = width / 2. - 1;
	}
	boost::multi_array<vector<Point>, 3> ellipseParametersSpace(boost::extents[width/centerStep+1][height/centerStep+1][(rMax - rMin)/rStep+1]);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (edges.At(x, y) > 0)
			{
				angle = directions.At(x, y);
				dx = cos(angle);
				dy = sin(angle);
				for (int r = 0; r < (rMax-rMin)/rStep; r++)
				{
					int shiftedR = r+rMin;
					int centerx = x + dx*shiftedR;
					int centery = y + dy*shiftedR;
					int centerx2 = x - dx*shiftedR;
					int centery2 = y - dy*shiftedR;
					if (centerx > 0 && centerx < width &&centery>0&&centery<height)
					{
						ellipseParametersSpace[centerx/centerStep][centery/centerStep][r]
							.push_back(Point(x, y));
					}
					if (centerx2 > 0 && centerx2 < width && centery2>0 && centery2<height)
					{
						ellipseParametersSpace[centerx2/centerStep][centery2/centerStep][r]
							.push_back(Point(x, y));
					}
				}
			}
		}
	}
	vector<vector<Point>> result;
	for (int y = 0; y < height/centerStep; y++)
	{
		for (int x = 0; x < width/centerStep; x++)
		{
			for (int r = 0; r < (rMax-rMin)/rStep; r++)
			{
				if (ellipseParametersSpace[x][y][r].size()>50)
				{
					result.push_back(ellipseParametersSpace[x][y][r]);
				}
			}
		}
	}
	return result;
}
double epsilon = 0.01;
bool PointBelongsToEllipse(const Point& point,const EllipseDescriptor& ellipse) 
{
	return abs(
		sqr((point.x - ellipse.x)*cos(ellipse.fi) + (point.y - ellipse.y) * sin(ellipse.fi))
		/ sqr(ellipse.a) +
		sqr((point.x - ellipse.x)*sin(ellipse.fi) - (point.y - ellipse.y)*cos(ellipse.fi))
		/ sqr(ellipse.b) - 1) < 2;
}

vector<EllipseDescriptor> HoughFeatureExtractor::FindEllipsesFast(const Matrix2D & edges, const Matrix2D & magnitude, const Matrix2D & directions, int rMin, int rMax)
{
	int height = edges.Height();
	int width = edges.Width();
	vector<EllipseDescriptor> result;
	vector<int> bAccumulator(rMax);
	
	vector<Point> edgesPoints;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (edges.At(x, y) > 1- epsilon)
			{
				edgesPoints.push_back(Point(x, y));
			}
		}
	}
	for (int p1Index = 0; p1Index < edgesPoints.size(); p1Index++)
	{
		double x1 = edgesPoints[p1Index].x;
		double y1 = edgesPoints[p1Index].y;
		for (int p2Index = p1Index+1; p2Index < edgesPoints.size(); p2Index++)
		{
			double x2 = edgesPoints[p2Index].x;
			double y2 = edgesPoints[p2Index].y;
			double a = hypot(x2-x1, y2-y1) / 2;
			if (!(a > rMin && a < rMax))
			{
				continue;
			}
			double x0 = (x1 + x2) / 2.;
			double y0 = (y1 + y2) / 2.;
			
			double slope = atan2(y2-y1, x2-x1);
			for (int p3Index = 0; p3Index < edgesPoints.size(); p3Index++)
			{
				double x3 = edgesPoints[p3Index].x;
				double y3 = edgesPoints[p3Index].y;
				if (p3Index == p2Index || p3Index == p1Index)
				{
					continue;
				}
				double d = hypot(x3 - x0, y3 - y0);
				double f = hypot(x3 - x2, y3 - y2);
				double cosTau = ((x3 - x0)*(x2 - x1) + (y3 - y0)*(y2 - y1)) / (2 * a*d);
				if (d < a && d>rMin)
				{
					double b = sqrt(sqr(a)*sqr(d)*(1-sqr(cosTau)) / (sqr(a) - sqr(d)*sqr(cosTau)));
					if (b<a && b < rMax && b>rMin)
					{
						bAccumulator[int(b)]++;
					}
				}
			}

			double max = DBL_MIN;
			int maxIndex = -1;
			for (int i = 0; i < rMax; i++)
			{
				if (bAccumulator[i] > max)
				{
					maxIndex = i;
					max = bAccumulator[i];
				}
			}
			if (maxIndex>0 && bAccumulator[maxIndex] > 150)
			{
				//auto bCandidates = bAccumulator[maxIndex];
				double b = maxIndex;//accumulate(bCandidates.begin(), bCandidates.end(), 0.) / bCandidates.size();
				auto foundEllipse = EllipseDescriptor(x0, y0, a, b, slope);
				result.push_back(foundEllipse);
				
				for (int i = 0; i < edgesPoints.size(); i++)
				{
					if (PointBelongsToEllipse(edgesPoints.at(i), foundEllipse)) 
					{
						edgesPoints.erase(edgesPoints.begin() + i);
						i--;
					}
				}				
			}
			for(auto& accumulator : bAccumulator)
			{
				accumulator = 0;
			}
		}


	}
	return result;

}



vector<vector<Point>> HoughFeatureExtractor::FindEllipses(const Matrix2D & edges, const Matrix2D & magnitude, const Matrix2D & directions)
{
	int width = magnitude.Width();
	int height = magnitude.Height();
	int roMax = hypot(width,height);
	int maxAngle = 180;
	int angleStep = 1;
	int rMax = 200;
	
	vector<vector<Point>> result;
	//fi,ro
	boost::multi_array<vector<Point>, 5> ellipseParametersSpace(boost::extents[maxAngle][width][height][rMax][rMax]);
	for (int slope = 0;slope < 180;slope++)
	{
		double slopeRad = slope / 180. * M_PI;
		for (int x0 = 0; x0 < width; x0++)
		{
			for (int y0 = 0; y0 < height; y0++)
			{
				for (int a = 0; a < rMax; a++)
				{
					for (int b = 0; b < a; b++)
					{
						for (int y = 0; y < edges.Height(); y++)
						{
							for (int x = 0; x < edges.Width(); x++)
							{
								if (edges.At(x, y) > 0) 
								{
									if (abs(
										sqr((x - x0)*cos(slopeRad) + (y - y0) * sin(slopeRad)) / sqr(a) +
										sqr((x - x0)*sin(slope) - (y - y0)*cos(slope)) / sqr(b) - 1) < epsilon)
									{
										ellipseParametersSpace[slope][x0][y0][a][b].push_back(Point(x,y));
									}
								}
							}
						}

					}
				}
			}
		}
	}
	for (int slope = 0;slope < 180;slope++)
	{
		double slopeRad = slope / 180. * M_PI;
		for (int x0 = 0; x0 < width; x0++)
		{
			for (int y0 = 0; y0 < height; y0++)
			{
				for (int a = 0; a < rMax; a++)
				{
					for (int b = 0; b < a; b++)
					{
						auto& points = ellipseParametersSpace[slope][x0][y0][a][b];
						if (points.size() > 5)
						{							
							result.push_back(points);
						}
					}
				}
			}
		}
	}
	return result;
}
