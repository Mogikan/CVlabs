#include "Image.h"
#include "DebugHelper.h"

Image::Image(uchar* rgbImageBytes, int width, int height,int bytesPerPixel)
{
	this->imageBytes = make_unique<uchar[]>(width*height);

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
	//NormalizeImage();
	this->width = width;
	this->height = height;
	this->bytesPerPixel = 1;
}

Image::Image(const unique_ptr<double[]>& normalizedPixels, int width, int height)
{
	bytesPerPixel = 1;
	auto rgbImage = make_unique<uchar[]>(width*height);	
	auto maxElement = *max(normalizedPixels.get(), normalizedPixels.get() + width*height);
	auto minElement = *min(normalizedPixels.get(), normalizedPixels.get() + width*height);

	transform(normalizedPixels.get(), normalizedPixels.get() + width*height, rgbImage.get(), [minElement,maxElement](double img)->uchar {return std::min((uchar)255,(uchar)((img-minElement) /(maxElement-minElement) * 255)); });
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

unique_ptr<double[]> Image::GetNormilizedDoubleData()
{
	auto normalizedData = make_unique<double[]>(width*height);

	transform(imageBytes.get(), imageBytes.get() + GetTotalBytes(), normalizedData.get(),[](uchar b)->double 
	{
		return b/255.0;
	});
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
	auto maxElement = *max(imageBytes.get(), imageBytes.get() + GetTotalBytes());
	auto minElement = *min(imageBytes.get(), imageBytes.get() + GetTotalBytes());
	
	transform(imageBytes.get(), imageBytes.get() + GetTotalBytes(), imageBytes.get(), [minElement, maxElement](uchar b)->uchar {return (uchar)((b - minElement) * 255 / (maxElement - minElement));});
}




