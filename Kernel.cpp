#include "Kernel.h"


unique_ptr < double[] > FillValues(double kernelValues[], int size)
{
	unique_ptr<double[]> values = make_unique<double[]>(size);
	copy(kernelValues, kernelValues + size*size, values.get());
	return move(values);
}


Kernel::Kernel(double values[], int kernelSize)
{
	this->kernelSize = kernelSize;
	this->values = make_unique<double[]>(kernelSize);
	copy(values, values + kernelSize*kernelSize, this->values.get());

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

double Kernel::GetElementAt(int x, int y)
{
	return values[y*kernelSize + x];
}

unique_ptr<Kernel> Kernel::CalculateSquare() const
{
	auto squareValues = new double[kernelSize*kernelSize];
	std::transform(values.get(), values.get() + kernelSize*kernelSize, squareValues, [](double x)->double { return x * x; });
	return unique_ptr<Kernel>(new Kernel(unique_ptr<double[]>(squareValues), this->kernelSize));
}

unique_ptr<Kernel> Kernel::CalculateSquareRoot() const
{
	auto squareValues = new double[kernelSize*kernelSize];
	std::transform(values.get(), values.get() + kernelSize*kernelSize, squareValues, [](double x)->double { return sqrt(x); });
	return unique_ptr<Kernel>(new Kernel(unique_ptr<double[]>(squareValues), this->kernelSize));
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
