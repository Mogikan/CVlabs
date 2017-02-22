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
	static unique_ptr<Matrix2D> Convolve(const unique_ptr<Matrix2D>& originalImage,const unique_ptr<Kernel>& kernel,ConvolutionBorderHandlingMode borderHandlingMode);
	static unique_ptr<Matrix2D> ApplySobelOperator(const unique_ptr<Matrix2D>& originalImage, ConvolutionBorderHandlingMode borderHandlingMode);
};
