#include "Kernel.h"
#include "MathHelper.h"

const Kernel Kernel::sobelX({
	1,0,-1,
	2,0,-2,
	1,0, -1,
}, 3, 3, Point(1, 1));

const Kernel Kernel::sobelY({
	1,2,1,
	0,0,0,
	-1,-2,-1,

}, 3, 3, Point(1, 1));

Kernel::Kernel(double values[], int width, int height,Point applicationPoint) 
	:Matrix2D(values, width, height)
{
	this->applicationPoint = applicationPoint;
}

Kernel::Kernel(vector<double> values, int width, int height,Point applicationPoint) 
	: Matrix2D(values, width, height)
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

Point Kernel::Center() const
{
	return applicationPoint;
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
		gaussKernelValues[i] = MathHelper::ComputeGaussAxesValue(i-kernelCenter,sigma);
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
		gaussKernelValues[i] = MathHelper::ComputeGaussAxesValue(i - kernelCenter, sigma);
	}
	return make_unique<Kernel>(gaussKernelValues, 1, kernelSize, kernelApplicationPoint);
}

const Kernel& Kernel::GetSobelX()
{	
	return sobelX;
}


const Kernel& Kernel::GetSobelY()
{
	return sobelY;
}

Kernel Kernel::GetDerivativeX()
{
	return Kernel({1, 0 , -1},3,1,Point(1,0));
}

Kernel Kernel::GetDerivativeY()
{
	return Kernel({1, 0 , -1},1,3,Point(0,1));
}

Kernel Kernel::GetDerivative()
{
	return Kernel(
	{
		0,1,0,
		1,0,-1,
		0,-1,0
	},3,3,Point(1,1));
}

