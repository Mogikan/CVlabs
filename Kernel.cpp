#include "Kernel.h"


unique_ptr < double[] > FillValues(double kernelValues[], int size)
{
	unique_ptr<double[]> values = make_unique<double[]>(size);
	copy(kernelValues, kernelValues + size, values.get());
	return move(values);
}


Kernel::Kernel(double values[], int kernelSize)
{
	this->kernelSize = kernelSize;
	this->values = make_unique<double[]>(kernelSize);
	copy(values, values + kernelSize, this->values.get());

}

Kernel::Kernel(unique_ptr<double[]> values, int kernelSize)
{
	this->kernelSize = kernelSize;
	this->values = move(values);
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
	return unique_ptr<Kernel>(new Kernel(FillValues(sobelYKernelValues,3),3));
}

int Kernel::GetTotalElements()
{
	return kernelSize*kernelSize;
}

int Kernel::GetRadius()
{
	return kernelSize/2;
}

int Kernel::GetElementAt(int x, int y)
{
	return values[y*kernelSize + x];
}

unique_ptr<Kernel> Kernel::operator*(double factor)
{
	auto multipliedValues = new double[kernelSize*kernelSize];
	transform(values.get(), values.get() + kernelSize*kernelSize, multipliedValues, [factor](double x)->double { return x * factor; });
	return unique_ptr<Kernel>(new Kernel(unique_ptr<double[]>(multipliedValues), this->kernelSize));
}

unique_ptr<Kernel> Kernel::CalculateSquare() const
{	
	auto squareValues = new double[kernelSize*kernelSize];
	transform(values.get(), values.get() +  kernelSize*kernelSize, squareValues, [](double x)->double { return x * x; });
	return unique_ptr<Kernel>(new Kernel(unique_ptr<double[]>(squareValues), this->kernelSize));
}

unique_ptr<Kernel> Kernel::CalculateSquareRoot() const
{
	auto squareValues = new double[kernelSize*kernelSize];
	transform(values.get(), values.get() + kernelSize*kernelSize, squareValues, [](double x)->double { return sqrt(x); });
	return unique_ptr<Kernel>(new Kernel(unique_ptr<double[]>(squareValues), this->kernelSize));
}

unique_ptr<Kernel> Kernel::operator+(unique_ptr<Kernel> secondKernel)
{
	auto sumValues = new double[kernelSize*kernelSize];
	transform(values.get(), values.get() + kernelSize * kernelSize, secondKernel->values.get(),sumValues, std::plus<double>());
	return unique_ptr<Kernel>(new Kernel(unique_ptr<double[]>(sumValues), this->kernelSize));
}



unique_ptr<Kernel> Kernel::GetSobelXKernel()
{
	double sobelXKernelValues[9] = {
		-1,0,1,
		-2,0,2,
		-1,0,1
	};
	
	return unique_ptr<Kernel>(new Kernel(FillValues(sobelXKernelValues,3),3));
}
