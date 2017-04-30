#pragma once
struct LineSpaceSettings
{
	LineSpaceSettings()
	{
	}
	LineSpaceSettings(double roMax, double roMin, double roStep, double angleStep)
	{
		this->roMax = roMax;
		this->roMin = roMin;
		this->roStep = roStep;
		this->angleStep = angleStep;
	}

	double roMax;
	double roMin;
	double roStep;
	double angleStep;
};

