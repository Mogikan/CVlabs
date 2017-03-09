#pragma once
#include "POIDetector.h"
class HarrisPOIDetector:public POIDetector 
{
public:
	HarrisPOIDetector();

	// Inherited via POIDetector
	virtual unique_ptr<Matrix2D> BuildHeatMap(Matrix2D & image) override;
protected:
	virtual double Threshold() override { return 0.05; };
};

