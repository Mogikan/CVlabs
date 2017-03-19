#pragma once
#include <memory>
#include <vector>
#include "Image.h"
#include "Kernel.h"
#include "Matrix2D.h"
#include "GaussPyramid.h"
#include "POIDetector.h"
#include "Descriptor.h"
class ImageFramework
{
public:
	ImageFramework();
	~ImageFramework();
	static unique_ptr<Matrix2D> Convolve(Matrix2D& originalImage,Kernel& kernel,BorderMode borderMode = BorderMode::extend);
	static unique_ptr<Matrix2D> ApplySobelOperator(Matrix2D& originalImage, BorderMode borderMode);
	static unique_ptr<Matrix2D> ApplySobelX(Matrix2D& originalImage, BorderMode borderHandlingMode = BorderMode::extend);
	static unique_ptr<Matrix2D> ApplySobelY(Matrix2D& originalImage, BorderMode borderHandlingMode = BorderMode::extend);
	static unique_ptr<Matrix2D> ApplyGaussSmooth(Matrix2D& image,double sigma);
	static unique_ptr<Matrix2D> DownscaleImageTwice(Matrix2D& image);
	static POIDetector CreatePOIDetector(POISearchMethod searchMethod);
	static unique_ptr<GaussPyramid> BuildGaussPyramid(Matrix2D& matrix,
		int octaveCount, 
		int octaveLayersCount,
		double sigma0, 
		double sigmaA);
};
