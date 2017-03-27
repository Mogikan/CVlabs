#pragma once
#include <vector>
#include "Matrix2D.h"
#include "Point.h"
#include <algorithm>
using namespace std;
class Descriptor:public vector<double>
{
public:	
	Descriptor(Point location, vector<double> value);	
	Point GetPoint();
private:
	const double Threshold = 0.2;
	Point location;			
	void Normalize();
	void RemoveNoise();

};

