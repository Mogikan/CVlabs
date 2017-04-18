#include "DescriptorService.h"
#include "ImageFramework.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "DebugHelper.h"
#include "MathHelper.h"
#include <assert.h>
#include "PointD.h"
static const double Threshold =0.8;
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
	auto smothedImage = ImageFramework::ApplyGaussSmooth(image, 1);

	auto dxdy = ImageFramework::Convolve(*smothedImage,Kernel::GetDerivative());
	for(Point point : interestingPoints)
	{
		descriptors.push_back(BuildAverageValueDescriptor(*dxdy, point,step,gridSize));
	}
	return descriptors;
}

vector<Descriptor> DescriptorService::BuildRotationInvariantDescriptors(
	const Matrix2D & image,
	vector<Point> interestingPoints,
	int step,
	int gridSize,
	int buckets,
	int mainDirectionBuckets)
{
	vector<Descriptor> descriptors;
	for (Point point : interestingPoints)
	{
		AddRotationInvariantDescriptors(descriptors, image, point, step, gridSize, buckets, mainDirectionBuckets);
	}
	return descriptors;
}

vector<Descriptor> DescriptorService::BuildGradientDirectionDescriptors(
	const Matrix2D & image, 
	const GaussPyramid& pyramid,
	const vector<BlobInfo>& blobs,
	int step, 
	int gridSize, 
	int buckets,
	int mainDirectionBuckets)
{
	vector<Descriptor> descriptors;
	int octaveCount = log2(min(image.Height(), image.Width()))-1;	
	for (const auto& blob:blobs)
	{
		AddGradientDirectionDescriptors(
			descriptors, 
			pyramid,
			blob, 
			step, 
			gridSize, 
			buckets, 
			mainDirectionBuckets);
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
	return Descriptor(descriptorValues, point);
}


vector<double> CalculateDescriptorValues(
	const Matrix2D& image,	
	Point point,
	int step,
	int gridSize,
	int buckets,
	double angle
)
{
	vector<double> descriptorValues(gridSize*gridSize*buckets, 0.0);
	double bucketAngleStep = 2 * M_PI / buckets;
	double halfBucketAngleStep = bucketAngleStep / 2;
	double centerX = gridSize*step / 2;
	double centerY = gridSize*step / 2;
	for (int pixelY = 0; pixelY < gridSize*step; pixelY++)
	{
		for (int pixelX = 0; pixelX < gridSize*step; pixelX++)
		{
			double gridDX = pixelX - centerX;
			double gridDY = pixelY - centerY;
			double pixelDistance = sqrt(gridDX*gridDX + gridDY*gridDY);
			double gaussWeight = 
				MathHelper::ComputeGaussAxesValue(pixelDistance, gridSize*step / 3./2.);
			double rotatedDX = gridDX*cos(angle) + gridDY*sin(angle);
			double rotatedDY = gridDY*cos(angle) - gridDX*sin(angle);
			

			int imageX = point.x + gridDX;
			int imageY = point.y + gridDY;
			double rotatedGridX = rotatedDX + centerX;
			double rightCellX = (rotatedGridX + step / 2) / step;
			int leftCellX = rightCellX - 1;
			double rotatedGridY = rotatedDY + centerY;
			double BottomCellY = (rotatedDY + centerY + step / 2) / step;
			int topCellY = BottomCellY - 1;
			int cellY;
			double cellCenterY;
			int cellX;
			double cellCenterX;
			for (int gridShiftY = 0; gridShiftY < 2; gridShiftY++)
			{
				cellY = topCellY + gridShiftY;
				cellCenterY = (cellY + 0.5)*step;				
				if (cellY < 0 || cellY >= gridSize)
				{
					continue;
				}
				double gridInterpolationWeightY = abs(cellCenterY - rotatedGridY) / step;
				for (int gridShiftX = 0; gridShiftX < 2; gridShiftX++)
				{
					cellX = leftCellX + gridShiftX;
					cellCenterX = (cellX + 0.5)*step;
					double gridInterpolationWeightX = abs(cellCenterX - rotatedGridX) / step;
					if (cellX<0 || cellX>=gridSize)
					{
						continue;
					}
					double dx = ImageFramework::SobelXAt(imageX, imageY, image);
					double dy = ImageFramework::SobelYAt(imageX, imageY, image);
					double derivativeLength = hypot(dx,dy);
					double fi = atan2(dy, dx) - angle;
					fi = fmod(fi + M_PI * 4, 2 * M_PI);
					int rightBucket = ((int)((fi + halfBucketAngleStep - DBL_EPSILON) / bucketAngleStep));
					int leftBucket = (rightBucket - 1);
					double leftBucketCenter = leftBucket*bucketAngleStep + halfBucketAngleStep;
					double leftBucketValuePart = 1 - abs(fi - leftBucketCenter) / bucketAngleStep;
					int descriptorOffset = (cellY*gridSize + cellX)*buckets;
					int leftBucketIndex = descriptorOffset + (leftBucket + buckets) % buckets;
					double totalWeight = gridInterpolationWeightX * gridInterpolationWeightY * gaussWeight;
					descriptorValues[leftBucketIndex] += leftBucketValuePart *totalWeight * derivativeLength;
					double rightValuePart = 1 - leftBucketValuePart;
					int rightBucketIndex = descriptorOffset + rightBucket%buckets;
					descriptorValues[rightBucketIndex] += rightValuePart*totalWeight *derivativeLength;
				}
			}
			
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

pair<double,double> InterpolateQuadratic(double leftValue, double value, double rightValue)
{
	double x = 0.5*(leftValue - rightValue) / (leftValue - 2 * value + rightValue);
	double y = value - 0.25*(leftValue - rightValue)*x;
	assert(x > -1 - DBL_EPSILON && x < 1 + DBL_EPSILON);
	return {x,y};
}



pair<PointD,PointD> FindTwoLocalMaximums(const vector<double>& values)
{	
	double max1Value = DBL_MIN;
	double max2Value = DBL_MIN;
	int max1Index = -1;
	int max2Index = -1;
	int n = values.size();
	for (int i = 0; i<values.size(); i++)
	{
		double value = values[i];
		double leftValue = values[(i - 1 + n) % n];
		double rightValue = values[(i + 1) % n];
		if (value > leftValue && value > rightValue)
		{
			if (value > max1Value)
			{
				max2Value = max1Value;
				max2Index = max1Index;
				max1Value = value;
				max1Index = i;
			}
			else if (value > max2Value)
			{
				max2Value = value;
				max2Index = i;
			}
		}
	}
	double left1Value = values[(max1Index - 1 + n) % n];
	double right1Value = values[(max1Index + 1 + n) % n];
	auto accurateShiftValue1 = InterpolateQuadratic(left1Value, max1Value, right1Value);
	double max1X = fmod(accurateShiftValue1.first + max1Index + n, n);
	assert(max1X > -DBL_EPSILON && max1X < 36 + DBL_EPSILON);
	double left2Value = values[(max2Index - 1 + n) % n];
	double right2Value = values[(max2Index + 1 + n) % n];
	auto accurateShiftValue2 = InterpolateQuadratic(left2Value, max2Value, right2Value);
	double max2X = fmod(accurateShiftValue2.first + max2Index + n, n);
	assert(max2X > -DBL_EPSILON && max2X < 36 + DBL_EPSILON);
	return
	{
		PointD(max1X,accurateShiftValue1.second),
		PointD(max2X,accurateShiftValue2.second)
	};
}

void DescriptorService::AddGradientDirectionDescriptors(
	vector<Descriptor>& targetDescriptors,
	const GaussPyramid& pyramid,
	BlobInfo blob,
	int step,
	int gridSize,
	int buckets,
	int mainDirectionBuckets)
{
	auto& blobLayer = pyramid.LayerAt(blob.octave,blob.layer);
	double blobStep = round(blobLayer.Sigma()*step);
	auto largeGridHistogram = CalculateDescriptorValues(
		blobLayer.GetImage(),
		blob.point, 
		blobStep*blobStep,
		1,
		mainDirectionBuckets,
		0);
	auto maximums = FindTwoLocalMaximums(largeGridHistogram);
	auto orientation1 = maximums.first.x*M_PI * 2 / mainDirectionBuckets;	
	double originalx = pow(2, blob.octave)*blob.point.x;
	double originaly = pow(2, blob.octave)*blob.point.y;
	targetDescriptors.push_back(
		Descriptor(			
			CalculateDescriptorValues(
				blobLayer.GetImage(),
				blob.point,
				blobStep,
				gridSize,
				buckets,
				orientation1
			),
			Point(originalx,originaly),
			orientation1,
			blobLayer.EffectiveSigma()
		)
	);
	if (maximums.second.y / maximums.first.y > Threshold)
	{
		double orientation2 = maximums.second.x*M_PI * 2 / mainDirectionBuckets;
		targetDescriptors.push_back(
			Descriptor(				
				CalculateDescriptorValues(
					blobLayer.GetImage(),
					blob.point,
					blobStep,
					gridSize,
					buckets,
					orientation2
					),
				Point(originalx, originaly),
				orientation2,
				blobLayer.EffectiveSigma()
			)
		);
	}	
}

void DescriptorService::AddRotationInvariantDescriptors(
	vector<Descriptor>& targetDescriptors,
	const Matrix2D& image,	
	Point point,
	int step,
	int gridSize,
	int buckets,
	int mainDirectionBuckets
)
{
	auto largeGridHistogram = CalculateDescriptorValues(
		image,		
		point,
		step*step,
		1,
		mainDirectionBuckets,
		0);
	auto maximums = FindTwoLocalMaximums(largeGridHistogram);
	targetDescriptors.push_back(
		Descriptor(			
			CalculateDescriptorValues(
				image,
				point,
				step,
				gridSize,
				buckets,
				maximums.first.x*M_PI * 2 / mainDirectionBuckets
			),
			point
		)
	);
	if (maximums.second.y / maximums.first.y > Threshold)
	{
		targetDescriptors.push_back(
			Descriptor(				
				CalculateDescriptorValues(
					image,
					point,
					step,
					gridSize,
					buckets,
					maximums.second.x*M_PI * 2 / mainDirectionBuckets
				)
				,point
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

bool MinimumSuits(TwoElementsResult minimums)
{
	return (minimums.firstElement / minimums.secondElement) < Threshold;
}

vector<pair<Descriptor, Descriptor>> DescriptorService::FindMatches(const vector<Descriptor>& descriptors1, const vector<Descriptor>& descriptors2)
{
	vector<pair<Descriptor, Descriptor>> matches;
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
					descriptor1,
					descriptor2
				});
			}
		}
	}
	return matches;
}


