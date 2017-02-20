#pragma once
#include <memory>
#include "Image.h"
#include "Kernel.h"
#include "Matrix2D.h"
enum ConvolutionBorderHandlingMode {wrap, mirror, extend, zero};
class ImageFramework
{
public:
	ImageFramework();
	~ImageFramework();
	static unique_ptr<Image> Convolve(const unique_ptr<Image>& originalImage,const unique_ptr<Kernel>& kernel,ConvolutionBorderHandlingMode borderHandlingMode);
};
