#include "Image.h"


Image::Image(uchar* rgbImageBytes, int width, int height,int bytesPerPixel)
{
	this->imageBytes = make_unique<uchar[]>(width*height);

	if (bytesPerPixel == 1)
	{		
		copy(rgbImageBytes, rgbImageBytes + width*height, this->imageBytes.get());
	}
	else 
	{
		for (long i = 0;i < width*height*bytesPerPixel;i += 3)
		{
			// 0.213R + 0.715G + 0.072B
			imageBytes[i] = 
				0.213 * rgbImageBytes[i * 3] + 
				0.715 * rgbImageBytes[i * 3 + 1] +
				0.072 * rgbImageBytes[i * 3 + 2];
		}
	}
	NormalizeImage();
	this->width = width;
	this->height = height;
	this->bytesPerPixel = 1;
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

unique_ptr<double[]> Image::GetNormilizedDoubleData()
{
	auto normalizedData = make_unique<double[]>(width*height);

	transform(imageBytes.get(), imageBytes.get() + GetTotalBytes(), normalizedData.get(),[](uchar b)->double {return b/255.0;});
	return normalizedData;
}

uchar* Image::GetRawData()
{
	uchar* result = new uchar[width*height*bytesPerPixel];
	copy(imageBytes.get(), imageBytes.get() +GetTotalBytes(), result);
	return result;
}

void Image::NormalizeImage()
{
	auto max = max_element(imageBytes.get(), imageBytes.get() + GetTotalBytes());
	auto min = min_element(imageBytes.get(), imageBytes.get() + GetTotalBytes());
	
	transform(imageBytes.get(), imageBytes.get() + GetTotalBytes(), imageBytes.get(), [min, max](uchar b)->uchar {return (uchar)((b - *min) * 255 / (*max - *min));});
}




