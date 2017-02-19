#pragma once
#include <stdlib.h>
#include <memory>
#include <algorithm>
using namespace std;
class Kernel
{
public:
	Kernel(int dimensionSize, double* values);
	~Kernel();
	static unique_ptr<Kernel> GetSobelXKernel();
	static unique_ptr<Kernel> GetSobelYKernel();
	int GetTotalElements();
	unique_ptr<Kernel> CalculateSquare() const;
	unique_ptr<Kernel> CalculateSquareRoot() const;
	unique_ptr<Kernel> operator+(unique_ptr<Kernel> secondKernel);
private:
	unique_ptr<double[]> values;
	int kernelSize;
};


