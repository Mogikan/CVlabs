#pragma once
#include "POIDetector.h"
class MoravecPOIDetector:public POIDetector 
{
public:
	MoravecPOIDetector();	
protected:
	virtual double Threshold() override { return 0.001; };
	virtual unique_ptr<Matrix2D> BuildHeatMap(Matrix2D & image) override;
private:
	double const shift = 1;		
	BorderMode const borderMode = BorderMode::extend;
	// Inherited via POIDetector
	
};

