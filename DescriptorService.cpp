#include "DescriptorService.h"
#include "ImageFramework.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "DebugHelper.h"
DescriptorService::DescriptorService()
{
}

vector<Descriptor> DescriptorService::BuildAverageValueDescriptors(
	const Matrix2D & image, 
	vector<Point> interestingPoints, 
	int step, 
	int gridSize)
{
	vector<Descriptor> descriptors;
	auto smothedImage = ImageFramework::ApplyGaussSmooth(image, 1.5);

	auto dxdy = ImageFramework::Convolve(*smothedImage,Kernel::GetDerivative());
	for(Point point : interestingPoints)
	{
		descriptors.push_back(BuildAverageValueDescriptor(*dxdy, point,step,gridSize));
	}
	return descriptors;
}

vector<Descriptor> DescriptorService::BuildGradientDirectionDescriptors(
	const Matrix2D & image, 
	vector<Point> interestingPoints,
	int step, 
	int gridSize, 
	int buckets,
	int mainDirectionBuckets)
{
	vector<Descriptor> descriptors;
	auto smothedImage = ImageFramework::ApplyGaussSmooth(image, 1.5);
	auto dxImage = ImageFramework::Convolve(*smothedImage, Kernel::GetDerivativeX());
	auto dyImage = ImageFramework::Convolve(*smothedImage, Kernel::GetDerivativeY());
	for (Point point : interestingPoints)
	{
		AddGradientDirectionDescriptors(descriptors, *dxImage, *dyImage, point, step, gridSize, buckets, mainDirectionBuckets);
	}
	return descriptors;
}

Descriptor DescriptorService::BuildAverageValueDescriptor(
	const Matrix2D & image, 
	Point point, 
	int step, 
	int gridSize)
{
	vector<double> descriptorValues(gridSize*gridSize,0.0);

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

vector<double> CalculateDescriptorValues(
	const Matrix2D& dxImage,
	const Matrix2D& dyImage,
	Point point,
	int step,
	int gridSize,
	int buckets,
	double angle
)
{
	vector<double> descriptorValues(gridSize*gridSize*buckets, 0.0);
	//int gridTop = point.y - gridSize / 2 * step + step - 1;
	//int gridLeft = point.x - gridSize / 2 * step + step - 1;
	double bucketAngleStep = 2 * M_PI / buckets;
	double halfBucketAngleStep = bucketAngleStep / 2;
	double centerX = gridSize*step / 2 - 1;
	double centerY = gridSize*step / 2 - 1;
	for (int pixelY = 0; pixelY < gridSize*step; pixelY++)
	{
		for (int pixelX = 0; pixelX < gridSize*step; pixelX++)
		{
			double netDX = pixelX - centerX;
			double netDY = pixelY - centerY;
			double rotatedDX = netDX*cos(angle) + netDY*sin(angle);
			double rotatedDY = netDY*cos(angle) - netDX*sin(angle);
			//DebugHelper::WriteToDebugOutput(rotatedDX);
			//DebugHelper::WriteToDebugOutput(rotatedDY);

			int imageX = point.x + rotatedDX;
			int imageY = point.y + rotatedDY;
			int cellX = pixelX / step;
			int cellY = pixelY / step;
			double dx = dxImage.GetIntensity(imageX, imageY);
			double dy = dyImage.GetIntensity(imageX, imageY);
			double derivativeLength = sqrt(dx*dx + dy*dy);
			double fi = atan2(dy, dx)-angle;
			fi = fmod(fi + M_PI * 4 , 2 * M_PI);
			int firstNearestBucket = (int)(fi / bucketAngleStep);
			double firstBucketCenter = firstNearestBucket*bucketAngleStep + halfBucketAngleStep;
			double firstBucketValuePart = 1 - abs(fi - firstBucketCenter) / bucketAngleStep;
			int firstBucketIndex = (cellY*gridSize + cellX)*buckets + firstNearestBucket;
			descriptorValues[firstBucketIndex] += firstBucketValuePart *derivativeLength;
			double secondValuePart = 1 - firstBucketValuePart;
			int secondNearestBucket;
			if (fi < halfBucketAngleStep)
			{
				secondNearestBucket = buckets - 1;
			}
			else  if (fi > 2 * M_PI - halfBucketAngleStep)
			{
				secondNearestBucket = 0;
			}
			else if (fi > firstNearestBucket * bucketAngleStep + halfBucketAngleStep)
			{
				secondNearestBucket = firstNearestBucket + 1;
			}
			else
			{
				secondNearestBucket = firstNearestBucket - 1;
			}
			int secondBucketIndex = (cellY*gridSize + cellX)*buckets + secondNearestBucket;
			descriptorValues[secondBucketIndex] += secondValuePart *derivativeLength;
		}
	}
	return descriptorValues;
}



struct TwoElementsResult
{
	TwoElementsResult(double firstElement, int firstElementIndex, double secondElement, int secondElementIndex) :
		firstElement(firstElement),
		firstElementIndex(firstElementIndex),
		secondElement(secondElement),
		secondElementIndex(secondElementIndex)
	{
	}
	double firstElement;
	int firstElementIndex;
	double secondElement;
	int secondElementIndex;
};

TwoElementsResult FindTwoMaximums(const vector<double>& values)
{	
	double firstMaximum = DBL_MIN;
	double secondMaximum = DBL_MIN;
	int firstMaximumIndex = -1;
	int secondMaximumIndex = -1;
	for (int i = 0; i<values.size(); i++)
	{
		double value = values[i];
		if (value > firstMaximum)
		{
			secondMaximum = firstMaximum;
			secondMaximumIndex = firstMaximumIndex;
			firstMaximum = value;
			firstMaximumIndex = i;
		}
		else if (value > secondMaximum)
		{
			secondMaximum = value;
			secondMaximumIndex = i;
		}
	}
	return TwoElementsResult(firstMaximum, firstMaximumIndex, secondMaximum, secondMaximumIndex);
}

void DescriptorService::AddGradientDirectionDescriptors(
	vector<Descriptor>& targetDescriptors,
	const Matrix2D& dxImage,
	const Matrix2D& dyImage, 
	Point point, 
	int step, 
	int gridSize,
	int buckets,
	int mainDirectionBuckets
)
{
	auto largeGridHistogram = CalculateDescriptorValues(
		dxImage, 
		dyImage, 
		point, 
		step, 
		1,
		mainDirectionBuckets,
		0);
	auto maximums = FindTwoMaximums(largeGridHistogram);
	targetDescriptors.push_back(
		Descriptor(
			point,
			CalculateDescriptorValues(
				dxImage,
				dyImage,
				point,
				step,
				gridSize,
				buckets,
				maximums.firstElementIndex*M_PI*2 / mainDirectionBuckets
			)
		)
	);
	if (maximums.secondElement / maximums.firstElement > 0.8)
	{
		targetDescriptors.push_back(
			Descriptor(
				point,
				CalculateDescriptorValues(
					dxImage,
					dyImage,
					point,				
					step,
					gridSize,
					buckets,
					maximums.secondElementIndex*M_PI * 2 / mainDirectionBuckets			
				)
			)
		);
	}	
}


double DescriptorService::CalculateDistance(Descriptor descriptor1, Descriptor descriptor2)
{
	double distance = 0;
	for (int i = 0; i < descriptor1.size(); i++)
	{
		distance += (descriptor1[i] - descriptor2[i])*(descriptor1[i] - descriptor2[i]);
	}
	return sqrt(distance);
}


TwoElementsResult FindTwoMinimums(Descriptor descriptor, const  vector<Descriptor>& descriptors)
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
	return TwoElementsResult(firstMinimum, firstMinimumIndex, secondMinimum, secondMinimumIndex);
}
const double Threshold = 0.8;
bool MinimumSuits(TwoElementsResult minimums)
{
	return (minimums.firstElement / minimums.secondElement) < Threshold;
}

vector<pair<Point, Point>> DescriptorService::FindMatches(const vector<Descriptor>& descriptors1, const vector<Descriptor>& descriptors2)
{
	vector<pair<Point, Point>> matches;
	for(int i=0;i<descriptors1.size();i++)
	{
		auto descriptor1 = descriptors1[i];
		auto minimums = FindTwoMinimums(descriptor1, descriptors2);
		if (MinimumSuits(minimums))
		{
			auto descriptor2 = descriptors2[minimums.firstElementIndex];
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


