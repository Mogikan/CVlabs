#pragma once
#include <memory>
#include <vector>
#include "Image.h"
#include "Kernel.h"
#include "Matrix2D.h"
#include "GaussPyramid.h"
enum ConvolutionBorderHandlingMode {wrap, mirror, extend, zero};
class ImageFramework
{
public:
	ImageFramework();
	~ImageFramework();
	static unique_ptr<Matrix2D> Convolve(Matrix2D& originalImage,Kernel& kernel,ConvolutionBorderHandlingMode borderHandlingMode);
	static unique_ptr<Matrix2D> ApplySobelOperator(Matrix2D& originalImage, ConvolutionBorderHandlingMode borderHandlingMode);
	static unique_ptr<Matrix2D> ApplyGaussSmooth(Matrix2D& image,double sigma);
	static unique_ptr<Matrix2D> DownscaleImageTwice(Matrix2D& image);
	static unique_ptr<GaussPyramid> BuildGaussPyramid(Matrix2D& matrix,int octaveCount, int octaveLayersCount,double sigma0, double sigmaA);
};
