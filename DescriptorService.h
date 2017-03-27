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
	vector<Descriptor> BuildAverageValueDescriptors(
		const Matrix2D& image, 
		vector<Point> interestingPoints,
		int step=4,
		int gridSize=4);

	vector<Descriptor> BuildGradientDirectionDescriptors(
		const Matrix2D& image, 
		vector<Point> interestingPoints,
		int step=4, 
		int gridSize=4,
		int buckets = 8,
		int mainDirectionBuckets = 36
	);

	vector<pair<Point, Point>> FindMatches(
		const vector<Descriptor>& descriptors1, 
		const vector<Descriptor>& descriptors2);

	static double CalculateDistance(
		Descriptor descriptor1, 
		Descriptor descriptor2);
private:
	Descriptor BuildAverageValueDescriptor(
		const Matrix2D& image,
		Point point,
		int step, 
		int gridSize);

	void AddGradientDirectionDescriptors(
		vector<Descriptor>& targetDescriptors,
		const Matrix2D& dxImage, 
		const Matrix2D& dyImage, 
		Point point,
		int step, 
		int gridSize,
		int buckets, 
		int mainDirectionBuckets);	
	
};

