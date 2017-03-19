#include "DescriptorService.h"
#include "ImageFramework.h"
#define _USE_MATH_DEFINES
#include <math.h>

DescriptorService::DescriptorService()
{
}

vector<Descriptor> DescriptorService::BuildAverageValueDescriptors(const Matrix2D & image, vector<Point> interestingPoints, int step, int gridSize)
{
	vector<Descriptor> descriptors;
	for each (Point point in interestingPoints)
	{
		descriptors.push_back(BuildAverageValueDescriptor(image, point));
	}
	return descriptors;
}

vector<Descriptor> DescriptorService::BuildGradientDirectionDescriptors(Matrix2D & image, vector<Point> interestingPoints,int step, int gridSize, int buckets)
{
	vector<Descriptor> descriptors;
	auto dxImage = ImageFramework::Convolve(image, Kernel::GetDerivativeX());
	auto dyImage = ImageFramework::Convolve(image, Kernel::GetDerivativeY());
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
			double fi = atan(dy / dx);
			double nearestBucketCenter = fi / bucketAngleStep;
		}
	}
}
