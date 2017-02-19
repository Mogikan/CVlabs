#include "Image.h"


Image::Image(uchar* rgbImageBytes, int width, int height,int bytesPerPixel)
{
	this->imageBytes = unique_ptr<uchar[]> (rgbImageBytes);
	this->width = width;
	this->height = height;
	this->bytesPerPixel = bytesPerPixel;
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

uchar* Image::GetRawData()
{
	uchar* result = new uchar[width*height*bytesPerPixel];
	copy(imageBytes.get(), imageBytes.get() +GetTotalBytes(), result);
	return result;
}




