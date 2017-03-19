#pragma once
#include "Matrix2D.h"
#include <vector>
#include "Descriptor.h"

using namespace std;
class DescriptorService
{
public:
	DescriptorService();
	vector<Descriptor> BuildAverageValueDescriptors(const Matrix2D& image, vector<Point> interestingPoints,int step=4,int gridSize=4);
	vector<Descriptor> BuildGradientDirectionDescriptors(Matrix2D& image, vector<Point> interestingPoints,int step=4, int gridSize=4,int buckets = 8);	
private:
	Descriptor BuildAverageValueDescriptor(const Matrix2D& image,Point point,int step, int gridSize);
	Descriptor BuildGradientDirectionDescriptor(Matrix2D& dxImage, Matrix2D& dyImage, Point point,int step, int gridSize,int buckets);
};

