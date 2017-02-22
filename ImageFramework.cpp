#include "ImageFramework.h"

ImageFramework::ImageFramework()
{
}


ImageFramework::~ImageFramework()
{
}

static double GetIntensity(const unique_ptr<Matrix2D>& image, int x, int y, ConvolutionBorderHandlingMode borderHandlingMode)
{
	int effectiveX = x;
	int effectiveY = y;
	int width = image->GetWidth();
	int height = image->GetHeight();
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
	return image->GetElementAt(effectiveX, effectiveY);
}




unique_ptr<Matrix2D> ImageFramework::Convolve(const unique_ptr<Matrix2D>& originalImageMatrix, const unique_ptr<Kernel>& kernel, ConvolutionBorderHandlingMode borderHandlingMode)
{	
	int width = originalImageMatrix->GetWidth();
	int height = originalImageMatrix->GetHeight();
	auto convolutedImageMatrix = unique_ptr<Matrix2D>(new Matrix2D(make_unique<double[]>(width*height), width, height));
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
		{
			double convolutionValue = 0;
			for (int yk = -kernel->GetYRadius(); yk < kernel->GetYRadius()+1; yk++)
				for (int xk = -kernel->GetXRadius(); xk < kernel->GetXRadius()+1; xk++)
				{	
					double imageIntensity = GetIntensity(originalImageMatrix, x + xk, y + yk, borderHandlingMode);
					double kernelValue = kernel->GetElementAt(kernel->GetXRadius()-xk, kernel->GetYRadius()- yk);
					convolutionValue += imageIntensity*kernelValue;				
				}
			convolutedImageMatrix->SetElementAt(x, y, convolutionValue);
		}
	return move(convolutedImageMatrix);
	//return unique_ptr<Image>(new Image(convolutedImageMatrix->ExtractData(),width, height));
}

unique_ptr<Matrix2D> ImageFramework::ApplySobelOperator(const unique_ptr<Matrix2D>& originalImage, ConvolutionBorderHandlingMode borderHandlingMode)
{
	auto sobelXImage = Convolve(originalImage, Kernel::GetSobelXKernel(), borderHandlingMode);
	int width = sobelXImage->GetWidth();
	int height = sobelXImage->GetHeight();
	long area = width*height;
	auto sobelXdoubleBitmap = sobelXImage->ExtractData();
	auto sobelYImage = Convolve(originalImage, Kernel::GetSobelYKernel(), borderHandlingMode);
	auto sobelYdoubleBitmap = sobelYImage->ExtractData();
	auto sobelOperatorResult = make_unique<double[]>(area);

	std::transform(sobelXdoubleBitmap.get(), sobelXdoubleBitmap.get() + area, sobelYdoubleBitmap.get(), sobelOperatorResult.get(),
		[](double sx, double sy)->double 
	{
		return sqrt(sx*sx + sy*sy);
	});
	return make_unique<Matrix2D>(move(sobelOperatorResult), width, height);
}
