#pragma once
struct Blob
{
	Blob(int x, int y, double r) 
	{
		this->x = x;
		this->y = y;
		this->r = r;
	}
	~Blob();
	int x;
	int y;
	double r;
};

