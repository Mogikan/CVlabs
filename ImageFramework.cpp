#include "ImageFramework.h"

ImageFramework::ImageFramework()
{
}


ImageFramework::~ImageFramework()
{
}

static double GetIntensity(Matrix2D& image, int x, int y, BorderMode borderHandlingMode)
{
	int effectiveX = x;
	int effectiveY = y;
	int width = image.Width();
	int height = image.Height();
	if (x < 0)
	{
		switch (borderHandlingMode)
		{
		case BorderMode::zero:
			return 0;
			break;
		case BorderMode::extend:
			effectiveX = 0;
			break;
		case BorderMode::mirror:
			effectiveX = -x - 1;
			break;
		case BorderMode::wrap:
			effectiveX = width + x;
			break;
		}
	}
	if (y < 0)
	{
		switch (borderHandlingMode)
		{
		case BorderMode::zero:
			return 0;
			break;
		case BorderMode::extend:
			effectiveY = 0;
			break;
		case BorderMode::mirror:
			effectiveY = -y - 1;
			break;
		case BorderMode::wrap:
			effectiveY = height + y;
			break;
		}
	}
	if (x >= width)
	{
		switch (borderHandlingMode)
		{
		case BorderMode::zero:
			return 0;
			break;
		case BorderMode::extend:
			effectiveX = width - 1;
			break;
		case BorderMode::mirror:
			effectiveX = width - 1 - (x - width);
			break;
		case BorderMode::wrap:
			effectiveX = x - width;
			break;
		}
	}
	if (y >= height)
	{
		switch (borderHandlingMode)
		{
		case BorderMode::zero:
			return 0;
			break;
		case BorderMode::extend:
			effectiveY = height - 1;
			break;
		case BorderMode::mirror:
			effectiveY = height - 1 - (x - height);
			break;
		case BorderMode::wrap:
			effectiveY = x - height;
			break;
		}
	}
	return image.GetElementAt(effectiveX, effectiveY);
}



unique_ptr<Matrix2D> ImageFramework::Convolve(
	Matrix2D& originalImageMatrix,
	Kernel& kernel, 
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
					double imageIntensity = GetIntensity(originalImageMatrix, x + xk, y + yk, borderHandlingMode);
					double kernelValue = kernel.GetElementAt(kernel.Center().x-xk, kernel.Center().y - yk);
					convolutionValue += imageIntensity*kernelValue;				
				}
			convolutedImageMatrix->SetElementAt(x, y, convolutionValue);
		}
	return move(convolutedImageMatrix);
}

unique_ptr<Matrix2D> ImageFramework::ApplySobelOperator(Matrix2D& originalImage, BorderMode borderHandlingMode)
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

unique_ptr<Matrix2D> ImageFramework::ApplyGaussSmooth(Matrix2D& image, double sigma)
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

unique_ptr<Matrix2D> ImageFramework::DownscaleImageTwice(Matrix2D & image)
{
	auto result = make_unique<Matrix2D>(image.Width()/2,image.Height()/2);
	for (int y = 0; y < result->Height(); y++)
	{
		for (int x = 0;x < result->Width();x++)
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



unique_ptr<GaussPyramid> ImageFramework::BuildGaussPyramid(
	Matrix2D& image,
	int octaveCount, 
	int layersInOctave, 
	double sigma0, 
	double sigmaA)
{
	return make_unique<GaussPyramid>(image,octaveCount,layersInOctave,sigma0, sigmaA);
}
