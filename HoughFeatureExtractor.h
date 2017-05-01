#pragma once
#include "Matrix2D.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Point.h"
#include "EllipseDescriptor.h"
#include "PlatformImageUtils.h"
#include "Size.h"
#include "DescriptorDimentionSettings.h"
#include "LineSpaceSettings.h"
#include "CircleSpaceSettings.h"
class HoughFeatureExtractor
{
public:
	HoughFeatureExtractor();
	static vector<vector<Point>> FindLines(
		const Matrix2D & edges, 
		const Matrix2D & magnitude, 
		const Matrix2D & directions,
		const LineSpaceSettings& lineSpaceSettings);
	static vector<CircleDescriptor> FindCircles(
		const Matrix2D & edges,
		const Matrix2D & magnitude,
		const Matrix2D & directions,
		const CircleSpaceSettings & settings);
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

	static pair<TransformationMetaInfo,vector<int>> FindObjectPose(
		Size objectSize,
		const vector<pair<Descriptor,Descriptor>>& matches,
		const DescriptorDimentionSettings& settings
	);

	~HoughFeatureExtractor();
};

