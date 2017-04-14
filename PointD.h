#pragma once
struct PointD
{
	PointD()
	{
	}
	PointD(double x, double y)
	{
		this->x = x;
		this->y = y;
	}
	double x;
	double y;
};