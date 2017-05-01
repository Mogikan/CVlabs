#pragma once
struct EllipseSpaceSettings
{
	EllipseSpaceSettings()
	{
	}
	EllipseSpaceSettings(
		double threshold,
		double fiMax,
		double fiMin,
		double fiStep,		
		double rMax,
		double rMin,		
		double rStep,
		double xMax,
		double xMin,
		double yMax,
		double yMin,
		double centerStep,
		double downscaleStep)
	{
		this->threshold = threshold;
		this->fiMax = fiMax;
		this->fiMin = fiMin;
		this->fiStep = fiStep;
		this->rStep = rStep;
		this->rMax = rMax;
		this->rMin = rMin;

		this->xMax = xMax;
		this->xMin = xMin;
		this->yMax = yMax;
		this->yMin = yMin;
		this->centerStep = centerStep;
		this->downscaleStep = downscaleStep;
	}
	double downscaleStep;
	double threshold;
	double fiMax;
	double fiMin;
	double fiStep;

	double rMax;
	double rMin;
	double rStep;
	
	double xMax;
	double xMin;
	double yMax;
	double yMin;
	double centerStep;
};

