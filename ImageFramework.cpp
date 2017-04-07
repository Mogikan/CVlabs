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


