#pragma once
#include <stdlib.h>
#include <utility>
#include "Matrix2D.h"
#define _USE_MATH_DEFINES
#include <math.h>
static double sqr(double a) { return a*a; }
class MathHelper
{
public:
	MathHelper();
	static std::pair<double,double> Eigenvalues(Matrix2D&);
	static double MathHelper::ComputeGaussAxesValue(int x, double sigma);
	~MathHelper();
};

