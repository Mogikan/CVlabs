#pragma once
#include <stdlib.h>
#include <utility>
#include "Matrix2D.h"
class MathHelper
{
public:
	MathHelper();
	static std::pair<double,double> Eigenvalues(Matrix2D&);
	~MathHelper();
};

