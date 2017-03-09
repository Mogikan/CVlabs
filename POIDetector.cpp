#include "POIDetector.h"
#include "ImageFramework.h"


POIDetector::POIDetector()
{
}

POIDetector::~POIDetector()
{
}

vector<Point> POIDetector::FindPoints(Matrix2D & image, bool suppressNonMaximum,int leftPointCount)
{
	auto smothedImage = ImageFramework::ApplyGaussSmooth(image, 1.5);
	auto specialPointsHeatMap = BuildHeatMap(*smothedImage);
	auto foundPoints = ChoosePeaks(*specialPointsHeatMap);
	if (suppressNonMaximum)
	{
		auto filteredPoints = SuppressNonMaximum(image, foundPoints, leftPointCount);
		return filteredPoints;
	}
	else 
	{
		return foundPoints;
	}

}

vector<Point> POIDetector::ChoosePeaks(Matrix2D & contrastMatrix)
{
	vector<Point> result;
	for (int y = 0; y < contrastMatrix.Height(); y++)
	{
		for (int x = 0; x < contrastMatrix.Width(); x++)
		{
			
			auto currentIntensity = contrastMatrix.GetIntensity(x, y);
			if (currentIntensity > Threshold()) 
			{
				if (CheckPointSuits(contrastMatrix, x, y))
				{
					result.push_back(Point(x, y));
				}
			}
		}
	}
	return result;
}

bool POIDetector::CheckPointSuits(const Matrix2D& image, int x, int y)
{
	int localMaxWindowHalf = localWindowSize / 2;
	double currentIntensity = image.GetIntensity(x,y);
	bool pointSuits = true;
	for (int dy = -localMaxWindowHalf; dy <= localMaxWindowHalf; dy++)
	{
		for (int dx = -localMaxWindowHalf; dx <= localMaxWindowHalf; dx++)
		{
			if (dx == 0 && dy == 0)
				continue;
			if (currentIntensity <= image.GetIntensity(x + dx, y + dy))
			{
				pointSuits = false;
				break;
			}
		}
		if (!pointSuits)
		{
			break;
		}
	}
	return pointSuits;
}

double DistanceSQR(Point first, Point second)
{
	double diffX = first.x - second.x;
	double diffY = first.y - second.y;
	return diffX*diffX + diffY*diffY;
}

vector<Point> POIDetector::SuppressNonMaximum(Matrix2D & heatMap, vector<Point> foundPoints,int leftPointsCount)
{
	vector<Point> result = foundPoints;
	double r = 0;
	while (result.size()>leftPointsCount)
	{
		for (int firstPointIndex = 0; firstPointIndex < result.size(); firstPointIndex++)
		{
			for (int secondPointIndex = firstPointIndex+1; secondPointIndex < result.size(); secondPointIndex++)
			{
				if (secondPointIndex == firstPointIndex)
				{
					continue;
				}
				auto firstPoint = result[firstPointIndex];
				auto secondPoint = result[secondPointIndex];
				if (DistanceSQR(firstPoint, secondPoint) < r*r)
				{
					if (heatMap.At(firstPoint.x, firstPoint.y) > heatMap.At(secondPoint.x, secondPoint.y))
					{
						result.erase(result.begin() + secondPointIndex);
						secondPointIndex = (secondPointIndex>0)? secondPointIndex--:0;
					}
					else 
					{
						result.erase(result.begin()+ firstPointIndex);
						firstPointIndex = (firstPointIndex>0)?firstPointIndex--:0;
						secondPointIndex = firstPointIndex + 1;
					}					
				}
			}
		}
		r += 0.9;
	}
	return result;
}
