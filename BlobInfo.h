#pragma once
#include "Point.h"
struct BlobInfo
{
	BlobInfo(int x, int y, int octave, int layer,int effectiveSigma) 
	{
		this->point = Point(x, y);		
		this->octave = octave;
		this->layer = layer;
		this->effectiveSigma = effectiveSigma;
	}
	Point point;
	int octave;
	int layer;
	double effectiveSigma;
};

