#pragma once
#include <vector>
#include "Matrix2D.h"
#include "Point.h"
#include <algorithm>
#include "TransformationMetaInfo.h"

using namespace std;
class Descriptor:public vector<double>
{
public:	
	Descriptor(vector<double> value, Point location,double angle = 0,double sigma = 0);
	Point GetPoint() const;
	double Angle() const;
	double Sigma() const;
	TransformationMetaInfo MetaInfo() const;
private:
	const double Threshold = 0.2;
	TransformationMetaInfo metaInfo;
	void Normalize();
	void RemoveNoise();

};

