#pragma once
#include "POIDetector.h"
class MoravecPOIDetector:public POIDetector 
{
public:
	MoravecPOIDetector();
	virtual vector<Point> FindPoints(const Matrix2D & image) override;
private:
	double const shift = 1;
	double const windowSize = 7;
	double const localWindowSize = 7;
	double const threshold = 0.05;
	BorderMode const borderMode = BorderMode::extend;
	// Inherited via POIDetector
	
};

