#pragma once
#include <vector>
#include "Matrix2D.h"
#include "Point.h"
#include <algorithm>
using namespace std;
class Descriptor
{
public:	
	Descriptor(Point location, vector<double> value) :location(location), descriptorValue(value) {};
	void Normalize();	
	int Size();
	double operator[](int index);
	Point GetPoint();
private:
	const double Threshold = 0.2;
	Point location;	
	void RemoveNoise();	
	vector<double> descriptorValue;
};

