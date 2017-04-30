#pragma once
#include <memory>
#include <vector>
#include "Image.h"
#include "Kernel.h"
#include "Matrix2D.h"
#include "GaussPyramid.h"
#include "POIDetector.h"
#include "Descriptor.h"
#define _USE_MATH_DEFINES
#include <math.h>
class ImageFramework
{
public:
	ImageFramework();
	~ImageFramework();
	static unique_ptr<Matrix2D> Convolve(
		const Matrix2D& originalImage,
		const Kernel& kernel,
		BorderMode borderMode = BorderMode::extend);

	static double ConvolveAt(
		const Matrix2D& originalImageMatrix,
		const Kernel& kernel,
		BorderMode borderHandlingMode,
		int x,
		int y
	);

	static unique_ptr<Matrix2D> ApplySobelOperator(
		const Matrix2D& originalImage, 
		BorderMode borderMode);

	static double SobelXAt(
		int x,
		int y,
		const Matrix2D& originalImage,
		BorderMode borderMode = BorderMode::extend
	);

	static double SobelYAt(
		int x,
		int y,
		const Matrix2D& originalImage,
		BorderMode borderMode = BorderMode::extend
	);


	static unique_ptr<Matrix2D> ApplySobelX(
		const Matrix2D& originalImage, 
		BorderMode borderHandlingMode = BorderMode::extend);

	static unique_ptr<Matrix2D> ApplySobelY(
		const Matrix2D& originalImage, 
		BorderMode borderHandlingMode = BorderMode::extend);

	static unique_ptr<Matrix2D> ApplyGaussSmooth(const Matrix2D& image,double sigma);

	static unique_ptr<Matrix2D> DownscaleImageTwice(const Matrix2D& image);

	static POIDetector CreatePOIDetector(POISearchMethod searchMethod);

	static unique_ptr<Matrix2D> ApplyCannyOperator(
		const Matrix2D& image,
		double lowerThreshold = 0.1,
		double upperThreshold = 0.3);

	static unique_ptr<Matrix2D> ApplyCannyOperator(
		const Matrix2D& direction,
		const Matrix2D& magnitude,
		double lowerThreshold = 0.07,
		double upperThreshold = 0.25);
};
