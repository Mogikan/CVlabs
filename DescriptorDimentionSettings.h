#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
struct DescriptorDimentionSettings
{
	DescriptorDimentionSettings()
	{
	}
	DescriptorDimentionSettings(
		int maxX, int minX, int stepX,
		int maxY, int minY, int stepY,
		double scaleMin, double scaleMax,
		double rotationStep = M_PI / 6.
	)
	{
		this->maxX = maxX;
		this->minX = minX;
		this->stepX = stepX;
		
		this->maxY = maxY;
		this->minY = minY;
		this->stepY = stepY;
		
		this->scaleMax = scaleMax;
		this->scaleMin = scaleMin;
		this->rotationStep = rotationStep;

			//this->scaleStep = scaleStep;
	}
	int maxX; 
	int minX; 
	int stepX;
	
	int maxY; 
	int minY; 
	int stepY;
	
	double scaleMin;
	double scaleMax; 
	//double scaleStep;
	
	double rotationStep;
};

