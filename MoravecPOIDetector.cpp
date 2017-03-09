#include "MoravecPOIDetector.h"
#include "PlatformImageUtils.h"


MoravecPOIDetector::MoravecPOIDetector()
{
}

unique_ptr<Matrix2D> MoravecPOIDetector::BuildHeatMap(Matrix2D & image)
{	
	auto minContrastMatrix = make_unique<Matrix2D>(image.Width(), image.Height());
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
					int windowHalfSize = HalfWindowSize();
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
			minContrastMatrix->SetElementAt(x, y, minContrast);
		}
	}
	PlatformImageUtils::SaveImage(Image(*minContrastMatrix), "C:\\moravecContrast.png");
	return move(minContrastMatrix);
}

