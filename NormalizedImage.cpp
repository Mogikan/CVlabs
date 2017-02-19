#include "NormalizedImage.h"



NormalizedImage::NormalizedImage(unique_ptr<double[]> image,int width, int height)
{
	this->imagePixels = move(image);
	this->width = width;
	this->height = height;

}


double NormalizedImage::GetElementAt(int x, int y)
{
	return imagePixels[y*width+x];
}

int NormalizedImage::GetWidth()
{
	return width;
}

int NormalizedImage::GetHeight()
{
	return height;
}

NormalizedImage::~NormalizedImage()
{
}
