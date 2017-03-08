#pragma once
#include "Image.h"
#include "Point.h"
#include <vector>
class POIDetector
{
public:
	POIDetector();
	virtual vector<Point> FindPoints(const Matrix2D& image)=0;
};

