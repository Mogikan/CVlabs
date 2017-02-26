#include "ImageFramework.h"

ImageFramework::ImageFramework()
{
}


ImageFramework::~ImageFramework()
{
}

static double GetIntensity(Matrix2D& image, int x, int y, ConvolutionBorderHandlingMode borderHandlingMode)
{
	int effectiveX = x;
	int effectiveY = y;
	int width = image.GetWidth();
	int height = image.GetHeight();
	if (x < 0)
	{
		switch (borderHandlingMode)
		{
		case ConvolutionBorderHandlingMode::zero:
			return 0;
			break;
		case ConvolutionBorderHandlingMode::extend:
			effectiveX = 0;
			break;
		case ConvolutionBorderHandlingMode::mirror:
			effectiveX = -x - 1;
			break;
		case ConvolutionBorderHandlingMode::wrap:
			effectiveX = width + x;
			break;
		}
	}
	if (y < 0)
	{
		switch (borderHandlingMode)
		{
		case ConvolutionBorderHandlingMode::zero:
			return 0;
			break;
		case ConvolutionBorderHandlingMode::extend:
			effectiveY = 0;
			break;
		case ConvolutionBorderHandlingMode::mirror:
			effectiveY = -y - 1;
			break;
		case ConvolutionBorderHandlingMode::wrap:
			effectiveY = height + y;
			break;
		}
	}
	if (x >= width)
	{
		switch (borderHandlingMode)
		{
		case ConvolutionBorderHandlingMode::zero:
			return 0;
			break;
		case ConvolutionBorderHandlingMode::extend:
			effectiveX = width - 1;
			break;
		case ConvolutionBorderHandlingMode::mirror:
			effectiveX = width - 1 - (x - width);
			break;
		case ConvolutionBorderHandlingMode::wrap:
			effectiveX = x - width;
			break;
		}
	}
	if (y >= height)
	{
		switch (borderHandlingMode)
		{
		case ConvolutionBorderHandlingMode::zero:
			return 0;
			break;
		case ConvolutionBorderHandlingMode::extend:
			effectiveY = height - 1;
			break;
		case ConvolutionBorderHandlingMode::mirror:
			effectiveY = height - 1 - (x - height);
			break;
		case ConvolutionBorderHandlingMode::wrap:
			effectiveY = x - height;
			break;
		}
	}
	return image.GetElementAt(effectiveX, effectiveY);
}



unique_ptr<Matrix2D> ImageFramework::Convolve(
	Matrix2D& originalImageMatrix,
	Kernel& kernel, 
	ConvolutionBorderHandlingMode borderHandlingMode)
{	
	int width = originalImageMatrix.GetWidth();
	int height = originalImageMatrix.GetHeight();
	auto convolutedImageMatrix = make_unique<Matrix2D>(width, height);
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
		{
			double convolutionValue = 0;
			for (int yk = -kernel.GetApplicationPoint().y; yk <  kernel.GetHeight() - kernel.GetApplicationPoint().y; yk++)
				for (int xk = -kernel.GetApplicationPoint().x; xk < kernel.GetWidth()- kernel.GetApplicationPoint().x; xk++)
				{	
					double imageIntensity = GetIntensity(originalImageMatrix, x + xk, y + yk, borderHandlingMode);
					double kernelValue = kernel.GetElementAt(kernel.GetApplicationPoint().x-xk, kernel.GetApplicationPoint().y - yk);
					convolutionValue += imageIntensity*kernelValue;				
				}
			convolutedImageMatrix->SetElementAt(x, y, convolutionValue);
		}
	return move(convolutedImageMatrix);
}

unique_ptr<Matrix2D> ImageFramework::ApplySobelOperator(Matrix2D& originalImage, ConvolutionBorderHandlingMode borderHandlingMode)
{
	auto sobelXImage = Convolve(originalImage, Kernel::GetSobelX(), borderHandlingMode);
	int width = sobelXImage->GetWidth();
	int height = sobelXImage->GetHeight();
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

unique_ptr<Matrix2D> ImageFramework::ApplyGaussSmooth(Matrix2D& image, double sigma)
{
	auto gausKernelX = Kernel::BuildGaussX(sigma);
	auto gausKernelY = Kernel::BuildGaussY(sigma);
	auto xSmoothedImage = ImageFramework::Convolve(
		image,
		*(gausKernelX.get()),
		ConvolutionBorderHandlingMode::extend);

	auto gaussImageMatrix = ImageFramework::Convolve(
		*xSmoothedImage,
		*(gausKernelY.get()), 
		ConvolutionBorderHandlingMode::extend);

	return gaussImageMatrix;
}

unique_ptr<Matrix2D> ImageFramework::DownscaleImageTwice(Matrix2D & image)
{
	auto result = make_unique<Matrix2D>(image.GetWidth()/2,image.GetHeight()/2);
	for (int y = 0; y < result->GetHeight(); y++)
	{
		for (int x = 0;x < result->GetWidth();x++)
		{
			result->SetElementAt(x, y,
				(
					image.GetElementAt(x * 2    , y * 2) +
					image.GetElementAt(x * 2 + 1, y * 2) +
					image.GetElementAt(x * 2    , y * 2 + 1) +
					image.GetElementAt(x * 2 + 1, y * 2 + 1)) / 4
			);
		}
	}
	return result;
}



unique_ptr<GaussPyramid> ImageFramework::BuildGaussPyramid(Matrix2D& image,int octaveCount, int layersInOctave, double sigma0, double sigmaA)
{
	return make_unique<GaussPyramid>(image,octaveCount,layersInOctave,sigma0, sigmaA);
}
