#pragma once
#include <stdlib.h>
#include <memory>
#include <algorithm>
using namespace std;
class Kernel
{
public:
	Kernel(double values[], int dimensionSize);
	Kernel(unique_ptr<double[]> values, int dimensionSize);
	~Kernel();
	static unique_ptr<Kernel> GetSobelXKernel();
	static unique_ptr<Kernel> GetSobelYKernel();
	int GetTotalElements();
	int GetRadius();
	double GetElementAt(int x, int y);	
	unique_ptr<Kernel> CalculateSquare() const;
	unique_ptr<Kernel> CalculateSquareRoot() const;	
	friend unique_ptr<Kernel> operator+(const unique_ptr<Kernel>& left, const unique_ptr<Kernel>& right);
	friend unique_ptr<Kernel> operator*(const unique_ptr<Kernel>& left,double factor);
private:
	unique_ptr<double[]> values;
	int kernelSize;
};
inline unique_ptr<Kernel> operator+(const unique_ptr<Kernel>& left, const unique_ptr<Kernel>& right)
{
	int kernelSize = left->kernelSize;
	auto sumValues = new double[kernelSize*kernelSize];
	std::transform(left->values.get(), left->values.get() + kernelSize * kernelSize, right->values.get(), sumValues, std::plus<double>());
	return unique_ptr<Kernel>(new Kernel(unique_ptr<double[]>(sumValues), kernelSize));
}

inline unique_ptr<Kernel> operator*(const unique_ptr<Kernel>& left, double factor)
{
	int kernelSize = left->kernelSize;
	auto multipliedValues = new double[kernelSize*kernelSize];
	std::transform(left->values.get(), left->values.get() + kernelSize*kernelSize, multipliedValues, [factor](double x)->double { return x * factor; });
	return unique_ptr<Kernel>(new Kernel(unique_ptr<double[]>(multipliedValues), kernelSize));
}

//auto multipliedValues = new double[kernelSize*kernelSize];
//transform(values.get(), values.get() + kernelSize*kernelSize, multipliedValues, [factor](double x)->double { return x * factor; });
//return *new Kernel(unique_ptr<double[]>(multipliedValues), this->kernelSize);


