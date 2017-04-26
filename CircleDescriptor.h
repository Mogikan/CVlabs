#pragma once
struct CircleDescriptor
{
	CircleDescriptor()
	{
	}
	CircleDescriptor(double x, double y, double r)
	{
		this->x = x;
		this->y = y;
		this->r = r;		
	}

	double x;
	double y;
	double r;	
};


