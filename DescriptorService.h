#pragma once
#include "Matrix2D.h"
#include <vector>
#include "Descriptor.h"
#include <utility>
using namespace std;
class DescriptorService
{
public:
	DescriptorService();
	vector<Descriptor> BuildAverageValueDescriptors(Matrix2D& image, vector<Point> interestingPoints,int step=4,int gridSize=4);
	vector<Descriptor> BuildGradientDirectionDescriptors(Matrix2D& image, vector<Point> interestingPoints,int step=4, int gridSize=4,int buckets = 8);	
	vector<pair<Point, Point>> FindMatches(vector<Descriptor> descriptors1, vector<Descriptor> descriptors2);
private:
	Descriptor BuildAverageValueDescriptor(const Matrix2D& image,Point point,int step, int gridSize);
	Descriptor BuildGradientDirectionDescriptor(Matrix2D& dxImage, Matrix2D& dyImage, Point point,int step, int gridSize,int buckets);	
	double CalculateDistance(Descriptor descriptor1, Descriptor descriptor2);
	const double Threshold = 0.8;
};

