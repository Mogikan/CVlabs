#include "Kernel.h"





Kernel::Kernel(int kernelSize, double * values)
{
	this->kernelSize = kernelSize;
	this->values = unique_ptr<double[]>(values);
}

Kernel::~Kernel()
{
}

unique_ptr<Kernel> Kernel::GetSobelYKernel()
{
	double sobelYKernelValues[9]{
		1,2,1,
		0,0,0,
		-1,-2,-1
	};
	return unique_ptr<Kernel>(new Kernel(3, sobelYKernelValues));
}

int Kernel::GetTotalElements()
{
	return kernelSize*kernelSize;
}

unique_ptr<Kernel> Kernel::CalculateSquare() const
{	
	auto squareValues = new double[kernelSize*kernelSize];
	transform(values.get(), values.get() +  kernelSize*kernelSize, squareValues, [](double x)->double { return x * x; });
	return unique_ptr<Kernel>(new Kernel(this->kernelSize, squareValues));
}

unique_ptr<Kernel> Kernel::CalculateSquareRoot() const
{
	auto squareValues = new double[kernelSize*kernelSize];
	transform(values.get(), values.get() + kernelSize*kernelSize, squareValues, [](double x)->double { return sqrt(x); });
	return unique_ptr<Kernel>(new Kernel(this->kernelSize, squareValues));
}

unique_ptr<Kernel> Kernel::operator+(unique_ptr<Kernel> secondKernel)
{
	auto sumValues = new double[kernelSize*kernelSize];
	transform(values.get(), values.get() + kernelSize * kernelSize, secondKernel->values.get(),sumValues, std::plus<double>());
	return unique_ptr<Kernel>(new Kernel(this->kernelSize, sumValues));
}



unique_ptr<Kernel> Kernel::GetSobelXKernel()
{
	double sobelXKernelValues[9] = {
		-1,0,1,
		-2,0,2,
		-1,0,1
	};
	return unique_ptr<Kernel>(new Kernel(3, sobelXKernelValues));
}
