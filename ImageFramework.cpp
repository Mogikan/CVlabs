#include "ImageFramework.h"



ImageFramework::ImageFramework()
{
}


ImageFramework::~ImageFramework()
{
}

unique_ptr<Image> ImageFramework::Convolve(unique_ptr<Image> originalImage, unique_ptr<Kernel> kernel)
{
	return nullptr;
}
