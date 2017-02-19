#pragma once
#include <memory>
#include "Image.h"
#include "Kernel.h"
class ImageFramework
{
public:
	ImageFramework();
	~ImageFramework();
	unique_ptr<Image> Convolve(unique_ptr<Image> originalImage,unique_ptr<Kernel> kernel);
};
