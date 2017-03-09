#include "HarrisPOIDetector.h"
#include "ImageFramework.h"
#include "MathHelper.h"
#include "PlatformImageUtils.h"

HarrisPOIDetector::HarrisPOIDetector()
{
}

unique_ptr<Matrix2D> HarrisPOIDetector::BuildHeatMap(Matrix2D & image)
{
	auto dx = ImageFramework::ApplySobelX(image);
	auto dy = ImageFramework::ApplySobelY(image);
	auto minContrastMatrix = make_unique<Matrix2D>(image.Width(), image.Height());
	for (int y = 0; y < image.Height(); y++)
	{
		for (int x = 0; x < image.Width(); x++)
		{
			Matrix2D H(2, 2);
			double a = 0;
			double b = 0;
			double c = 0;
			for (int shiftY = -HalfWindowSize(); shiftY <= HalfWindowSize(); shiftY++)
			{
				for (int shiftX = -HalfWindowSize(); shiftX <= HalfWindowSize(); shiftX++)
				{
					double Ix = dx->GetIntensity(x + shiftX, y + shiftY);
					double Iy = dy->GetIntensity(x + shiftX, y + shiftY);
					a += Ix*Ix;
					b += Ix*Iy;
					c += Iy*Iy;
				}
			}
			H.SetElementAt(0, 0, a);
			H.SetElementAt(0, 1, b);
			H.SetElementAt(1, 0, b);
			H.SetElementAt(1, 1, c);
			auto eigenvalues = MathHelper::Eigenvalues(H);
			auto lambdaMin = min(eigenvalues.first, eigenvalues.second);
			minContrastMatrix->SetElementAt(x, y, lambdaMin);
		}
	}
	//PlatformImageUtils::SaveImage(Image(*minContrastMatrix), "C:\\harrisLambdas.png");
	return move(minContrastMatrix);
}


