#include "Kernel.h"

Kernel::Kernel(double values[], int width, int height) :Matrix2D(values, width, height)
{
}

Kernel::Kernel(unique_ptr<double[]> values, int width, int height) : Matrix2D(move(values), width, height)
{
}

Kernel::~Kernel()
{
}

unique_ptr<Kernel> Kernel::GetSobelYKernel()
{
	double sobelYKernelValues[]{
		-1,-2,-1,
		0,0,0,
		1,2,1
	};
	return make_unique<Kernel>(sobelYKernelValues,3,3);
}



int Kernel::GetXRadius()
{
	return GetWidth()/2;
}

int Kernel::GetYRadius()
{
	return GetHeight() / 2;
}

unique_ptr<Kernel> Kernel::GetSobelXKernel()
{
	double sobelXKernelValues[] = {
		-1,0,1,
		-2,0,2,
		-1,0,1
	};
	
	return make_unique<Kernel>(sobelXKernelValues,3,3);
}
