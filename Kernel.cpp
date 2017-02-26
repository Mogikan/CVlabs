#include "Kernel.h"
#define _USE_MATH_DEFINES
#include <math.h>


Kernel::Kernel(double values[], int width, int height,Point applicationPoint) :Matrix2D(values, width, height)
{
	this->applicationPoint = applicationPoint;
}

Kernel::Kernel(vector<double> values, int width, int height,Point applicationPoint) : Matrix2D(values, width, height)
{
	this->applicationPoint = applicationPoint;
}

Kernel::Kernel(const Kernel & kernel):Matrix2D(kernel)
{
	applicationPoint = kernel.applicationPoint;
}

Kernel::~Kernel()
{
}

Point Kernel::GetApplicationPoint()
{
	return applicationPoint;
}

double ComputeGaussAxesValue(int x,double sigma) 
{
	return 1 / (sqrt(2 * M_PI)*sigma)*exp(-x*x / (2 * sigma*sigma));
}

int ComputeGaussKernelSize(double sigma) 
{
	return (int)(3 * sigma) * 2 + 1;
}

unique_ptr<Kernel> Kernel::BuildGaussX(double sigma)
{
	int kernelSize = ComputeGaussKernelSize(sigma);	
	int kernelCenter = kernelSize / 2;
	Point kernelApplicationPoint = Point(kernelCenter,0);
	auto gaussKernelValues = vector<double>(kernelSize);
	for (int i = 0; i < kernelSize; i++)
	{
		gaussKernelValues[i] = ComputeGaussAxesValue(i-kernelCenter,sigma);
	}
	return make_unique<Kernel>(gaussKernelValues, kernelSize, 1, kernelApplicationPoint);
}

unique_ptr<Kernel> Kernel::BuildGaussY(double sigma)
{
	int kernelSize = ComputeGaussKernelSize(sigma);
	int kernelCenter = kernelSize / 2;
	Point kernelApplicationPoint = Point(0, kernelCenter);
	auto gaussKernelValues = vector<double>(kernelSize);
	for (int i = 0; i < kernelSize; i++)
	{
		gaussKernelValues[i] = ComputeGaussAxesValue(i - kernelCenter, sigma);
	}
	return make_unique<Kernel>(gaussKernelValues, 1, kernelSize, kernelApplicationPoint);
}

Kernel Kernel::GetSobelX()
{
	double sobelXKernelValues[] = {
		1,0,-1,
		2,0,-2,
		1,0, - 1,
	};	
	Kernel sobelX(sobelXKernelValues, 3, 3, Point(1, 1));
	return sobelX;
}


Kernel Kernel::GetSobelY()
{
	double sobelYKernelValues[]{
		-1,-2,-1,
		0,0,0,
		1,2,1,

	};
	Kernel sobelY(sobelYKernelValues, 3, 3, Point(1, 1));
	return sobelY;
}

