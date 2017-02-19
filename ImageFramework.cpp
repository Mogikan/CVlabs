#include "ImageFramework.h"



ImageFramework::ImageFramework()
{
}


ImageFramework::~ImageFramework()
{
}

double GetIntensity(const unique_ptr<NormalizedImage>& image,int x, int y, ConvolutionBorderHandlingMode borderHandlingMode)
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
			effectiveX = width -1 - (x - width);
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
			effectiveX = height - 1;
			break;
		case ConvolutionBorderHandlingMode::mirror:
			effectiveX = height - 1 - (x - height);
			break;
		case ConvolutionBorderHandlingMode::wrap:
			effectiveX = x - height;
			break;
		}
	}
	return image->GetElementAt(effectiveX, effectiveY);	
}

unique_ptr<Image> ImageFramework::Convolve(unique_ptr<Image> originalImage, unique_ptr<Kernel> kernel,ConvolutionBorderHandlingMode borderHandlingMode)
{
	auto normalizedDoubleImage = originalImage->GetNormilizedDoubleData();
	int width = originalImage->GetWidth();
	int height = originalImage->GetHeight();
	auto normalizedImage = make_unique<NormalizedImage>(new NormalizedImage(move(normalizedDoubleImage), originalImage->GetWidth(), originalImage->GetHeight()));
	//TODO: create new doubleImage
	for (int y = 0;y < height ;y++)
	for (int x = 0; x < width; x++)
	{
		double convolutionValue = 0;
		for (int yk = -kernel->GetRadius();yk<kernel->GetRadius();yk++)
		for (int xk = -kernel->GetRadius();xk < kernel->GetRadius();xk++)
		{
			double imageIntensity = GetIntensity(normalizedImage, x+xk,y+yk,borderHandlingMode);
			double kernelValue = kernel->GetElementAt(xk + kernel->GetRadius(), yk + kernel->GetRadius());
			convolutionValue += imageIntensity*kernelValue;
		}
	//TODO: set new doubleImage value to convolutionValue 	
	}	
}
