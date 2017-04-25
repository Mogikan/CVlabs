#pragma once
#include "Matrix2D.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Point.h"
#include "EllipseDescriptor.h"
class HoughFeatureExtractor
{
public:
	HoughFeatureExtractor();
	static vector<vector<Point>> FindLines(
		const Matrix2D & edges, 
		const Matrix2D & magnitude, 
		const Matrix2D & directions);
	static vector<vector<Point>> FindCircles(
		const Matrix2D & edges,
		const Matrix2D & magnitude,
		const Matrix2D & directions,
		int rMin = 10,
		int rMax= 300);
	static vector<EllipseDescriptor> FindEllipsesFast
	(
		const Matrix2D & edges,
		const Matrix2D & magnitude,
		const Matrix2D & directions,
		int rMin = 5,
		int rMax = 300);

	static vector<vector<Point>> FindEllipses(
		const Matrix2D & edges,
		const Matrix2D & magnitude,
		const Matrix2D & directions);
	~HoughFeatureExtractor();
};

