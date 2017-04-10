#pragma once
#include "Matrix2D.h"
#include "Descriptor.h"
#include <unordered_set>


class Ransac
{
public:
	Ransac();
	static Matrix2D FindBestHomography(const vector<pair<Descriptor,Descriptor>>& matches);
	~Ransac();
};

