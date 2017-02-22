#pragma once
#include <stdlib.h>
#include <memory>
#include <algorithm>
#include <Matrix2D.h>
using namespace std;
class Kernel : public Matrix2D
{
public:	
	Kernel(double values[], int width, int height);
	Kernel(unique_ptr<double[]> values, int width, int height);
	~Kernel();
	static unique_ptr<Kernel> GetSobelXKernel();
	static unique_ptr<Kernel> GetSobelYKernel();	
	int GetXRadius();
	int GetYRadius();	
};