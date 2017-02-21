#include "Image.h"
#include "DebugHelper.h"

Image::Image(uchar* rgbImageBytes, int width, int height,int bytesPerPixel)
{
	this->imageBytes = make_unique<uchar[]>(width*height);
	this->width = width;
	this->height = height;	
	if (bytesPerPixel == 1)
	{		
		copy(rgbImageBytes, rgbImageBytes + width*height, this->imageBytes.get());
	}
	else 
	{
		for (long i = 0;i < width*height;i++)
		{
			// 0.213R + 0.715G + 0.072B
			imageBytes[i] = (uchar)min(255.,
				0.213 * rgbImageBytes[i * 3] + 
				0.715 * rgbImageBytes[i * 3 + 1] +
				0.072 * rgbImageBytes[i * 3 + 2]);
		}		
	}	
	this->bytesPerPixel = 1;
}

Image::Image(const unique_ptr<double[]>& normalizedPixels, int width, int height)
{
	bytesPerPixel = 1;
	auto rgbImage = make_unique<uchar[]>(width*height);	
	auto maxElement = *max_element(normalizedPixels.get(), normalizedPixels.get() + width*height);
	auto minElement = *min_element(normalizedPixels.get(), normalizedPixels.get() + width*height);

	transform(normalizedPixels.get(), normalizedPixels.get() + width*height, rgbImage.get(), 
		[minElement,maxElement](double img)->uchar 
	{		
		return (uchar)std::min(255.,((img-minElement) /(maxElement-minElement) * 255.)); 
	});
	auto maxElement1 = *max_element(rgbImage.get(), rgbImage.get() + width*height);
	auto minElement1 = *min_element(rgbImage.get(), rgbImage.get() + width*height);

	DebugHelper::WriteToDebugOutput(maxElement1);
	DebugHelper::WriteToDebugOutput(minElement1);
	imageBytes = move(rgbImage);
	this->width = width;
	this->height = height;	
}

Image::~Image()
{
}

int Image::GetWidth() const
{
	return width;
}

int Image::GetHeight() const
{
	return height;
}

int Image::GetBytesPerPixel() const
{
	return bytesPerPixel;
}

int Image::GetTotalBytes() const
{
	return width*height*bytesPerPixel;
}

unique_ptr<double[]> Image::GetDoubleData()
{
	auto doubleData = make_unique<double[]>(width*height);
	transform(imageBytes.get(), imageBytes.get() + GetTotalBytes(), doubleData.get(), [](uchar b)->double
	{
		return b;
	});
	return doubleData;
}

unique_ptr<double[]> Image::GetNormilizedDoubleData()
{
	auto normalizedData = make_unique<double[]>(width*height);
	auto maxElement = *max_element(imageBytes.get(), imageBytes.get() + GetTotalBytes());
	auto minElement = *min_element(imageBytes.get(), imageBytes.get() + GetTotalBytes());

	transform(imageBytes.get(), imageBytes.get() + GetTotalBytes(), normalizedData.get(), [minElement, maxElement](uchar b)->double 
	{
		//DebugHelper::WriteToDebugOutput((double)(b - minElement) / (double)(maxElement - minElement));
		return (double)(b - minElement) / (double)(maxElement - minElement);
	});

	
	return normalizedData;
}

uchar* Image::GetRawData()
{
	uchar* result = new uchar[GetTotalBytes()];
	copy(imageBytes.get(), imageBytes.get() +GetTotalBytes(), result);
	return result;
}

void Image::NormalizeImage()
{
	auto maxElement = *max_element(imageBytes.get(), imageBytes.get() + GetTotalBytes());
	auto minElement = *min_element(imageBytes.get(), imageBytes.get() + GetTotalBytes());
	
	transform(imageBytes.get(), imageBytes.get() + GetTotalBytes(), imageBytes.get(), [minElement, maxElement](uchar b)->uchar {return (uchar)(255.*(b - minElement) / (double)(maxElement - minElement));});
}




