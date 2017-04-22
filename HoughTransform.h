#pragma once
#include "Matrix2D.h"
class HoughTransform
{
public:
	HoughTransform();
	void Apply(const Matrix2D& edges, const Matrix2D& magnitude, const Matrix2D& directions);
	~HoughTransform();
};

