#include "DescriptorService.h"
#include "ImageFramework.h"
#define _USE_MATH_DEFINES
#include <math.h>

DescriptorService::DescriptorService()
{
}

vector<Descriptor> DescriptorService::BuildAverageValueDescriptors(Matrix2D & image, vector<Point> interestingPoints, int step, int gridSize)
{
	vector<Descriptor> descriptors;
	auto smothedImage = ImageFramework::ApplyGaussSmooth(image, 1.5);

	auto dxdy = ImageFramework::Convolve(*smothedImage,Kernel::GetDerivative());
	for each (Point point in interestingPoints)
	{
		descriptors.push_back(BuildAverageValueDescriptor(*dxdy, point,step,gridSize));
	}
	return descriptors;
}

vector<Descriptor> DescriptorService::BuildGradientDirectionDescriptors(Matrix2D & image, vector<Point> interestingPoints,int step, int gridSize, int buckets)
{
	vector<Descriptor> descriptors;
	auto smothedImage = ImageFramework::ApplyGaussSmooth(image, 1.5);
	auto dxImage = ImageFramework::Convolve(*smothedImage, Kernel::GetDerivativeX());
	auto dyImage = ImageFramework::Convolve(*smothedImage, Kernel::GetDerivativeY());
	for each (Point point in interestingPoints)
	{
		descriptors.push_back(BuildGradientDirectionDescriptor(*dxImage, *dyImage, point, step, gridSize, buckets));
	}
	return descriptors;
}

Descriptor DescriptorService::BuildAverageValueDescriptor(const Matrix2D & image, Point point, int step, int gridSize)
{
	vector<double> descriptorValues;
	for (int i = 0; i < gridSize*gridSize; i++)
	{
		descriptorValues.push_back(0);
	}

	int gridTop = point.y - gridSize / 2 * step + step - 1;
	int gridLeft = point.x - gridSize / 2 * step + step - 1;
	for (int pixelY = 0; pixelY < gridSize*step; pixelY++)
	{
		for (int pixelX = 0; pixelX < gridSize*step;pixelX++)
		{
			int imageX = pixelX + gridLeft;
			int imageY = pixelY + gridTop;
			int cellX = pixelX / step;
			int cellY = pixelY / step;
			descriptorValues[cellY * gridSize + cellX] += image.GetIntensity(imageX,imageY);
		}
	}
	return Descriptor(point, descriptorValues);
}

Descriptor DescriptorService::BuildGradientDirectionDescriptor(Matrix2D& dxImage,Matrix2D& dyImage, Point point, int step, int gridSize,int buckets)
{
	vector<double> descriptorValues;
	for (int i = 0; i < gridSize * gridSize * buckets; i++)
	{
		descriptorValues.push_back(0);
	}
	int gridTop = point.y - gridSize / 2 * step + step - 1;
	int gridLeft = point.x - gridSize / 2 * step + step - 1;
	double bucketAngleStep = 2 * M_PI / buckets;
	double halfBucketAngleStep = bucketAngleStep /2;
	for (int pixelY = 0; pixelY < gridSize*step; pixelY++)
	{
		for (int pixelX = 0; pixelX < gridSize*step;pixelX++)
		{
			int imageX = pixelX + gridLeft;
			int imageY = pixelY + gridTop;
			int cellX = pixelX / step;
			int cellY = pixelY / step;
			double dx = dxImage.GetIntensity(imageX, imageY);
			double dy = dyImage.GetIntensity(imageX, imageY);
			double derivativeLength = sqrt(dx*dx + dy*dy);
			double fi = atan2(dy, dx);
			if (fi < 0)
				fi = 2 * M_PI + fi;
			int firstNearestBucket = (int)(fi / bucketAngleStep);			
			double firstBucketCenter = firstNearestBucket*bucketAngleStep + halfBucketAngleStep;
			double firstBucketValuePart = 1 - abs(fi - firstBucketCenter)/bucketAngleStep;
			int firstBucketIndex = (cellY*gridSize + cellX)*buckets + firstNearestBucket;
			descriptorValues[firstBucketIndex] = firstBucketValuePart *derivativeLength;
			double secondValuePart = 1 - firstBucketValuePart;
			int secondNearestBucket;
			if (fi < halfBucketAngleStep)
			{
				secondNearestBucket = buckets - 1;
			}
			else  if (fi > 2*M_PI - halfBucketAngleStep)
			{
				secondNearestBucket = 0;
			}
			else if (fi > firstNearestBucket * bucketAngleStep + halfBucketAngleStep)
			{
				secondNearestBucket = firstNearestBucket+1;
			}
			else 
			{
				secondNearestBucket = firstNearestBucket-1;
			}
			int secondBucketIndex = (cellY*gridSize + cellX)*buckets + secondNearestBucket;
			descriptorValues[secondBucketIndex] = secondValuePart *derivativeLength;			
		}
	}
	return Descriptor(point, descriptorValues);
}

double DescriptorService::CalculateDistance(Descriptor descriptor1, Descriptor descriptor2)
{
	double distance = 0;
	for (int i = 0; i < descriptor1.Size(); i++)
	{
		distance += (descriptor1[i] - descriptor2[i])*(descriptor1[i] - descriptor2[i]);
	}
	return sqrt(distance);
}

struct MinimumResult 
{
	MinimumResult(double firstMinimum,int firstMinimumIndex, double secondMinimum, int secondMinimumIndex):
		firstMinimum(firstMinimum),
		firstMinimumIndex(firstMinimumIndex),
		secondMinimum(secondMinimum),
		secondMinimumIndex(secondMinimumIndex)
	{
	}
	double firstMinimum;
	int firstMinimumIndex;
	double secondMinimum;
	int secondMinimumIndex;
};

MinimumResult FindTwoMinimums(Descriptor descriptor, vector<Descriptor> descriptors)
{
	double firstMinimum = DBL_MAX;
	double secondMinimum = DBL_MAX;
	int firstMinimumIndex = -1;
	int secondMinimumIndex = -1;
	for (int i = 0;i<descriptors.size();i++)
	{
		auto descriptor2 = descriptors[i];
		double distance = DescriptorService::CalculateDistance(descriptor, descriptor2);
		if (distance < firstMinimum)
		{
			secondMinimum = firstMinimum;
			secondMinimumIndex = firstMinimumIndex;
			firstMinimum = distance;
			firstMinimumIndex = i;
		}
		else if (distance < secondMinimum)
		{
			secondMinimum = distance;
			secondMinimumIndex = i;
		}
	}
	return MinimumResult(firstMinimum, firstMinimumIndex, secondMinimum, secondMinimumIndex);
}
const double Threshold = 0.8;
bool MinimumSuits(MinimumResult minimums)
{
	return (minimums.firstMinimum / minimums.secondMinimum) < Threshold;
}

vector<pair<Point, Point>> DescriptorService::FindMatches(vector<Descriptor> descriptors1, vector<Descriptor> descriptors2)
{
	vector<pair<Point, Point>> matches;
	for(int i=0;i<descriptors1.size();i++)
	{
		auto descriptor1 = descriptors1[i];
		auto minimums = FindTwoMinimums(descriptor1, descriptors2);
		if (MinimumSuits(minimums))
		{
			auto descriptor2 = descriptors2[minimums.firstMinimumIndex];
			auto backwardMinimums = FindTwoMinimums(descriptor2, descriptors1);
			if (MinimumSuits(backwardMinimums))
			{
				matches.push_back(
				{
					descriptor1.GetPoint(),
					descriptor2.GetPoint()
				});
			}
		}

	}
	
	return matches;
}


