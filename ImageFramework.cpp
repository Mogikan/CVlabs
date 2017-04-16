#include "ImageFramework.h"

ImageFramework::ImageFramework()
{
}


ImageFramework::~ImageFramework()
{
}


unique_ptr<Matrix2D> ImageFramework::Convolve(
	const Matrix2D& originalImageMatrix,
	const Kernel& kernel, 
	BorderMode borderHandlingMode)
{	
	int width = originalImageMatrix.Width();
	int height = originalImageMatrix.Height();
	auto convolutedImageMatrix = make_unique<Matrix2D>(width, height);
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
		{
			double convolutionValue = 0;
			for (int yk = -kernel.Center().y; yk <  kernel.Height() - kernel.Center().y; yk++)
				for (int xk = -kernel.Center().x; xk < kernel.Width()- kernel.Center().x; xk++)
				{	
					double imageIntensity = originalImageMatrix.GetIntensity(x + xk, y + yk, borderHandlingMode);
					double kernelValue = kernel.At(kernel.Center().x-xk, kernel.Center().y - yk);
					convolutionValue += imageIntensity*kernelValue;				
				}
			convolutedImageMatrix->SetElementAt(x, y, convolutionValue);
		}
	return move(convolutedImageMatrix);
}

double ImageFramework::ConvolveAt(
	const Matrix2D& originalImageMatrix,
	const Kernel& kernel,
	BorderMode borderHandlingMode,
	int x,
	int y
)
{
	int width = originalImageMatrix.Width();
	int height = originalImageMatrix.Height();
	double convolutionValue = 0;
	for (int yk = -kernel.Center().y; yk < kernel.Height() - kernel.Center().y; yk++)
		for (int xk = -kernel.Center().x; xk < kernel.Width() - kernel.Center().x; xk++)
		{
			double imageIntensity = originalImageMatrix.GetIntensity(x + xk, y + yk, borderHandlingMode);
			double kernelValue = kernel.At(kernel.Center().x - xk, kernel.Center().y - yk);
			convolutionValue += imageIntensity*kernelValue;
		}
	return convolutionValue;
}


unique_ptr<Matrix2D> ImageFramework::ApplySobelOperator(const Matrix2D& originalImage, BorderMode borderHandlingMode)
{
	auto sobelXImage = Convolve(originalImage, Kernel::GetSobelX(), borderHandlingMode);
	int width = sobelXImage->Width();
	int height = sobelXImage->Height();
	long area = width*height;
	auto sobelXdoubleBitmap = sobelXImage->ExtractData();
	auto sobelYImage = Convolve(originalImage, Kernel::GetSobelY(), borderHandlingMode);
	auto sobelYdoubleBitmap = sobelYImage->ExtractData();
	auto sobelOperatorResult = vector<double>(area);
	
	std::transform(begin(sobelXdoubleBitmap), 
		end(sobelXdoubleBitmap), 
		begin(sobelYdoubleBitmap), 
		begin(sobelOperatorResult),
		[](double sx, double sy)->double 
	{
		return sqrt(sx*sx + sy*sy);
	});
	return make_unique<Matrix2D>(sobelOperatorResult, width, height);
}

double ImageFramework::SobelXAt(int x, int y, const Matrix2D & originalImage, BorderMode borderMode)
{
	return ConvolveAt(originalImage, Kernel::GetSobelX(), BorderMode::extend, x, y);
}

double ImageFramework::SobelYAt(int x, int y, const Matrix2D & originalImage, BorderMode borderMode)
{
	return ConvolveAt(originalImage, Kernel::GetSobelY(), BorderMode::extend, x, y);
}

unique_ptr<Matrix2D> ImageFramework::ApplySobelX(const Matrix2D & originalImage, BorderMode borderHandlingMode)
{
	return Convolve(originalImage, Kernel::GetSobelX(), borderHandlingMode);
}

unique_ptr<Matrix2D> ImageFramework::ApplySobelY(const Matrix2D & originalImage, BorderMode borderHandlingMode) 
{
	return Convolve(originalImage, Kernel::GetSobelY(), borderHandlingMode);;
}

unique_ptr<Matrix2D> ImageFramework::ApplyGaussSmooth(const Matrix2D& image, double sigma)
{
	auto gausKernelX = Kernel::BuildGaussX(sigma);
	auto gausKernelY = Kernel::BuildGaussY(sigma);
	auto xSmoothedImage = ImageFramework::Convolve(
		image,
		*(gausKernelX.get()),
		BorderMode::extend);

	auto gaussImageMatrix = ImageFramework::Convolve(
		*xSmoothedImage,
		*(gausKernelY.get()), 
		BorderMode::extend);

	return gaussImageMatrix;
}

unique_ptr<Matrix2D> ImageFramework::DownscaleImageTwice(const Matrix2D & image)
{
	auto result = make_unique<Matrix2D>(image.Width()/2,image.Height()/2);
	for (int y = 0; y < result->Height(); y++)
	{
		for (int x = 0;x < result->Width();x++)
		{
			result->SetElementAt(x, y,
				(
					image.At(x * 2    , y * 2) +
					image.At(x * 2 + 1, y * 2) +
					image.At(x * 2    , y * 2 + 1) +
					image.At(x * 2 + 1, y * 2 + 1)) / 4
			);
		}
	}
	return result;
}

POIDetector ImageFramework::CreatePOIDetector(POISearchMethod searchMethod)
{
	return POIDetector(searchMethod);
}

int CalculateDiscreteDirection(double gradient)
{
	double pointGradient = gradient;
	pointGradient += M_PI;//0 to 2PI
	pointGradient /= M_PI / 8; //0 to 15
	pointGradient++; //1 to 16			
	int direction = pointGradient / 2;//0 to 8
	return direction % 4; //0 to 3
}

unique_ptr<Matrix2D> ImageFramework::ApplyCannyOperator(const Matrix2D & image, double lowerThreshold, double upperThreshold)
{
	auto smoothedImage = ApplyGaussSmooth(image, 1.4);
	auto sobelX = ApplySobelX(*smoothedImage);
	auto sobelY = ApplySobelY(*smoothedImage);
	auto magnitude = Matrix2D(image.Width(), image.Height());
	auto direction = Matrix2D(image.Width(), image.Height());
	for (int y = 0; y < image.Height(); y++)
	{
		for (int x = 0; x < image.Width(); x++)
		{
			double dx = sobelX->At(x, y);
			double dy = sobelY->At(x, y);
			magnitude.SetElementAt(x, y, hypot(dx, dy));
			direction.SetElementAt(x, y, atan2(dy, dx));//-PI to PI
		}
	}
	auto edges = Matrix2D(image.Width(), image.Height());
	for (int y = 0; y < image.Height(); y++)
	{
		for (int x = 0; x < image.Width(); x++)
		{		
			double pointGradient = direction.At(x, y);	
			int pointDirection = CalculateDiscreteDirection(pointGradient) / 2;
			int dx = pointDirection == 2 ? 0 : 1;
			int dy = pointDirection == 0 ? 0 : 1;
			double currentMagnitude = magnitude.GetIntensity(x,y);
			edges.SetElementAt(x, y,
				currentMagnitude > upperThreshold &&
				currentMagnitude > magnitude.GetIntensity(x - dx, y - dy) &&
				currentMagnitude > magnitude.GetIntensity(x + dx, y + dy));
		}
	}
	vector<bool> notWatchedTrueEdges(image.Width()*image.Height(),true);
	int width = image.Width();
	int height = image.Height();
	bool imageChanged;
	do
	{
		imageChanged = false;
		for (int y = 0; y < image.Height(); y++)
		{
			for (int x = 0; x < image.Width(); x++)
			{
				if (!(edges.At(x, y) > DBL_EPSILON))
				{
					continue;
				}
				if (!(notWatchedTrueEdges[y*width + x]))
				{
					continue;
				}
				notWatchedTrueEdges[y*width + x] = false;
				double pointGradient = direction.At(x, y);
				int pointDirection = CalculateDiscreteDirection(pointGradient);
				int dx = pointDirection == 2 ? 0 : 1;
				int dy = pointDirection == 0 ? 0 : 1;
				int normaldx = dy;
				int normaldy = dx;
				int neighbor1x = x - dx;
				int neighbor1y = y - dy;
				double neighbor1Magnitude = magnitude.GetIntensity(neighbor1x, neighbor1y);
				if (CalculateDiscreteDirection(direction.GetIntensity(neighbor1x, neighbor1y)) == pointDirection&&
					neighbor1x > 0 && neighbor1x < width &&
					neighbor1y>0 && neighbor1y < height &&
					neighbor1Magnitude > lowerThreshold&&
					neighbor1Magnitude > magnitude.GetIntensity(neighbor1x - normaldx, neighbor1y - normaldy) &&
					neighbor1Magnitude > magnitude.GetIntensity(neighbor1x + normaldx, neighbor1y + normaldy)
					)
				{
					imageChanged = true;
					edges.SetElementAt(neighbor1x, neighbor1y, 1);
				}

				int neighbor2x = x + dx;
				int neighbor2y = y + dy;
				double neighbor2Magnitude = magnitude.GetIntensity(neighbor2x, neighbor2y);

				if (CalculateDiscreteDirection(direction.GetIntensity(neighbor2x, neighbor2y)) == pointDirection&&
					neighbor2x > 0 && neighbor2x < width &&
					neighbor2y > 0 && neighbor2y < height &&
					neighbor2Magnitude > lowerThreshold&&
					neighbor2Magnitude > magnitude.GetIntensity(neighbor2x - normaldx, neighbor2y - normaldy) &&
					neighbor2Magnitude > magnitude.GetIntensity(neighbor2x + normaldx, neighbor2y + normaldy)
					)
				{
					imageChanged = true;
					edges.SetElementAt(neighbor2x, neighbor2y, 1);
				}
			}
		}		
	} 
	while (imageChanged);
	return make_unique<Matrix2D>(edges);

}


