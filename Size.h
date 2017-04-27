#pragma once
#include "Point.h"
struct Size
{
	Size()
	{
	}
	Size(int w, int h)
	{
		this->w = w;
		this->h = h;
	}
	Point Center() { return Point(w / 2, h / 2); }
	int w;
	int h;
};