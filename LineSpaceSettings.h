#pragma once
struct LineSpaceSettings
{
	LineSpaceSettings()
	{
	}
	LineSpaceSettings(double roMax, double roMin, double roStep, double angleStep,double threshold)
	{
		this->roMax = roMax;
		this->roMin = roMin;
		this->roStep = roStep;
		this->angleStep = angleStep;
		this->threshold = threshold;
	}
	double threshold;
	double roMax;
	double roMin;
	double roStep;
	double angleStep;
};

