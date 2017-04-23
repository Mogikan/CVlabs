#include "HoughFeatureExtractor.h"
#include "boost/multi_array.hpp"
#include "MathHelper.h"

HoughFeatureExtractor::HoughFeatureExtractor()
{
}


HoughFeatureExtractor::~HoughFeatureExtractor()
{
}

vector<vector<Point>> HoughFeatureExtractor::FindLines(const Matrix2D & edges, const Matrix2D & magnitude, const Matrix2D & directions)
{
	int roMax = hypot(magnitude.Width(), magnitude.Height());
	int maxAngle = 360;
	int angleStep = 1;
	//fi,ro
	boost::multi_array<vector<Point>, 2> lineParametersSpace(boost::extents[roMax*2][maxAngle/angleStep]);

	
	for (int y = 0; y < edges.Height(); y++)
	{
		for (int x = 0; x < edges.Width(); x++)
		{
			if (edges.At(x, y) > 0)
			{
				double angleRadians = directions.At(x, y);
				double angleRadNormalized = fmod(angleRadians+2 * M_PI,2 * M_PI);
				int fi = int(angleRadNormalized/M_PI*180);
				int height = edges.Height();
				int width = edges.Width();
				int ro = ((x-width/2.)*cos(angleRadians)+(y-height/2.)*sin(angleRadians))+roMax/2;
				lineParametersSpace[ro][int(fi)].push_back(Point(x, y));
			}
		}
	}
	vector<vector<Point>> result;
	for (int ro = 0; ro < roMax*2; ro++)
	{
		for (int fi = 0; fi < maxAngle; fi++)
		{
			auto accumulatedLines = lineParametersSpace[ro][fi];
			if (accumulatedLines.size() > 5
				)
			{				
				result.push_back(accumulatedLines);
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
	int rMax = 300;
	double epsilon = 0.01;
	vector<vector<Point>> result;
	//fi,ro
	boost::multi_array<vector<Point>, 5> ellipseParametersSpace(boost::extents[maxAngle][width+2*rMax][height*2*rMax][rMax][rMax]);
	for (int slope = 0;slope < 180;slope++)
	{
		double slopeRad = slope / 180. * M_PI;
		for (int x0 = -rMax; x0 < width+rMax; x0++)
		{
			for (int y0 = -rMax; y0 < height + rMax; y0++)
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
										Sqr((x - x0)*cos(slopeRad) + (y - y0) * sin(slopeRad)) / Sqr(a) +
										Sqr((x - x0)*sin(slope) - (y - y0)*cos(slope)) / Sqr(b) - 1) < epsilon)
									{
										ellipseParametersSpace[slope][x0 + rMax][y0 + rMax][a][b].push_back(Point(x,y));
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
		for (int x0 = -rMax; x0 < width + rMax; x0++)
		{
			for (int y0 = -rMax; y0 < height + rMax; y0++)
			{
				for (int a = 0; a < rMax; a++)
				{
					for (int b = 0; b < a; b++)
					{
						auto& points = ellipseParametersSpace[slope][x0 + rMax][y0 + rMax][a][b];
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
