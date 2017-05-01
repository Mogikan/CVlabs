#pragma once
struct CircleSpaceSettings
{
	CircleSpaceSettings()
	{
	}
	CircleSpaceSettings(double rMax, double rMin, double rStep, double centerStep)
	{
		this->rMax = rMax;
		this->rMin = rMin;		
		this->rStep = rStep;
		this->centerStep = centerStep;
	}

	double rMin;
	double rMax;
	double rStep;
	double centerStep;
};

