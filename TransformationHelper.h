#pragma once
#include "Matrix2D.h"
#include "Descriptor.h"
#include <unordered_set>


class TransformationHelper
{
public:
	TransformationHelper();
	static Matrix2D FindBestHomography(const vector<pair<Descriptor,Descriptor>>& matches,int N=500, double pointAccuracy=4);
};

