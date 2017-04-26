#pragma once
#include "Matrix2D.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Point.h"
#include "EllipseDescriptor.h"
#include "PlatformImageUtils.h"
class HoughFeatureExtractor
{
public:
	HoughFeatureExtractor();
	static vector<vector<Point>> FindLines(
		const Matrix2D & edges, 
		const Matrix2D & magnitude, 
		const Matrix2D & directions,
		int roStep = 5,
		int angleStep=4);
	static vector<CircleDescriptor> FindCircles(
		const Matrix2D & edges,
		const Matrix2D & magnitude,
		const Matrix2D & directions,
		int rMin = 1,
		int rMax= 300, 
		int rStep = 2,
		int centerStep = 4);
	static vector<EllipseDescriptor> FindEllipsesFast
	(
		const Matrix2D & edges,
		const Matrix2D & magnitude,
		const Matrix2D & directions,
		int rMin = 5,
		int rMax = 150,
		double rStep = 1,
		int edgeStep =4
		);

	static vector<vector<Point>> FindEllipses(
		const Matrix2D & edges,
		const Matrix2D & magnitude,
		const Matrix2D & directions);

	static void FindPose() 
	{

	}

	~HoughFeatureExtractor();
};

