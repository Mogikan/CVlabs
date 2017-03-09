#include "POIDetector.h"
#include "ImageFramework.h"


POIDetector::POIDetector()
{
}

POIDetector::~POIDetector()
{
}

vector<Point> POIDetector::FindPoints(Matrix2D & image)
{
	auto smothedImage = ImageFramework::ApplyGaussSmooth(image, 1.5);
	auto specialPointsHeatMap = BuildHeatMap(*smothedImage);
	return ChoosePeaks(*specialPointsHeatMap);
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