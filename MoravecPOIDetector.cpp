#include "MoravecPOIDetector.h"
#include "PlatformImageUtils.h"


MoravecPOIDetector::MoravecPOIDetector()
{
}

vector<Point> MoravecPOIDetector::FindPoints(const Matrix2D & image)
{
	vector<Point> result;
	Matrix2D minContrastMatrix(image.Width(), image.Height());
	for (int y = 0; y < image.Height();y++)
	{
		for (int x = 0; x < image.Width();x++)
		{
			double minContrast = DBL_MAX;
			for (int shiftDirectionX = -1; shiftDirectionX <= 1;shiftDirectionX++)
			{
				for (int shiftDirectionY = -1; shiftDirectionY <= 1;shiftDirectionY++)
				{
					if (shiftDirectionX == 0 && shiftDirectionY == 0)
						continue;
					int shiftedX = x + shift*shiftDirectionX;
					int shiftedY = y + shift*shiftDirectionY;
					int windowHalfSize = windowSize / 2;
					double currentContrast = 0;
					for (int windowY = -windowHalfSize;windowY <= windowHalfSize;windowY++)
					{
						for (int windowX = -windowHalfSize;windowX <= windowHalfSize;windowX++)
						{
						
							int pixelX = x + windowX;
							int pixelY = y + windowY;
							int shiftedPixelX = shiftedX + windowX;
							int shiftedPixelY = shiftedY + windowY;
							double difference = image.GetIntensity(pixelX, pixelY) - image.GetIntensity(shiftedPixelX, shiftedPixelY);
							currentContrast += difference*difference;
						}
					}
					if (currentContrast < minContrast)					
						 minContrast = currentContrast;
				}
			}
			minContrastMatrix.SetElementAt(x, y, minContrast);
		}
	}
	PlatformImageUtils::SaveImage(Image(minContrastMatrix), "C:\\diff.png");
	for (int y = 0; y < minContrastMatrix.Height(); y++)
	{
		for (int x = 0; x < minContrastMatrix.Width(); x++)
		{
			int localMaxWindowHalf = localWindowSize / 2;
			auto currentIntensity = minContrastMatrix.GetIntensity(x, y);
			if (currentIntensity > threshold) {
				bool pointSuits = true;
				for (int dy = -localMaxWindowHalf; dy <= localMaxWindowHalf; dy++)
				{
					for (int dx = -localMaxWindowHalf; dx <= localMaxWindowHalf; dx++)
					{
						if (dx == 0 && dy == 0)
							continue;
						if (currentIntensity <= minContrastMatrix.GetIntensity(x + dx, y + dy))
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
				if (pointSuits)
				{
					result.push_back(Point(x, y));
				}
			}
		}
	}
	return result;
}

