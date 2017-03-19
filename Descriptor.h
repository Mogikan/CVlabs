#pragma once
#include <vector>
#include "Matrix2D.h"
#include "Point.h"
using namespace std;
class Descriptor
{
public:
	Descriptor(Point location, vector<double> value):location(location), descriptorValue(value) {};
private:
	Point location;
	vector<double> descriptorValue;
};

