#pragma once
struct EllipseDescriptor
{
	EllipseDescriptor()
	{
	}
	EllipseDescriptor(double x, double y, double a, double b, double fi)
	{
		this->x = x;
		this->y = y;
		this->a = a;
		this->b = b;
		this->fi = fi;
	}
	
	double x;
	double y;
	double a;
	double b;
	double fi;

};


