#pragma once
#include <stdlib.h>
#include <utility>
#include "Matrix2D.h"
#define _USE_MATH_DEFINES
#include <math.h>
class MathHelper
{
public:
	MathHelper();
	static std::pair<double,double> Eigenvalues(Matrix2D&);
	static double MathHelper::ComputeGaussAxesValue(int x, double sigma);
	static double Sqr(double x) { return x*x; }
	~MathHelper();
};

