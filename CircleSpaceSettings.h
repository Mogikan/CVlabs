#pragma once
struct CircleSpaceSettings
{
	CircleSpaceSettings()
	{
	}
	CircleSpaceSettings(
		double rMax, 
		double rMin, 
		double rStep, 
		double centerStep,
		double xMin,
		double xMax,
		double yMin,
		double yMax)
	{
		this->rMax = rMax;
		this->rMin = rMin;		
		this->rStep = rStep;
		this->xMin = xMin;
		this->xMax = xMax;
		this->yMin = yMin;
		this->yMax = yMax;
		
		this->centerStep = centerStep;
	}

	double rMin;
	double rMax;
	double rStep;
	double centerStep;
	double xMin;
	double xMax;
	double yMin;
	double yMax;
};

