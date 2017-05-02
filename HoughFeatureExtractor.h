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
#include "EllipseSpaceSettings.h"
#include "LineDescriptor.h"
class HoughFeatureExtractor
{
public:
	HoughFeatureExtractor();
	static vector<pair<vector<Point>, LineDescriptor>> FindLines(
		const Matrix2D & edges, 
		const Matrix2D & magnitude, 
		const Matrix2D & directions,
		const LineSpaceSettings& lineSpaceSettings);
	static vector<pair<Point, Point>> FindLineSegments(
		const vector<pair<vector<Point>, LineDescriptor>>& linePoints,		
		//LineSpaceSettings settings,
		Size imageSize,
		double threshold);
	
	static vector<CircleDescriptor> FindCircles(
		const Matrix2D & edges,
		const Matrix2D & magnitude,
		const Matrix2D & directions,
		const CircleSpaceSettings & settings);
	static vector<EllipseDescriptor> FindEllipses
	(
		const Matrix2D & edges,
		const Matrix2D & magnitude,
		const Matrix2D & directions,
		const EllipseSpaceSettings& settings
		);

	static pair<TransformationMetaInfo,vector<int>> FindObjectPose(
		Size objectSize,
		const vector<pair<Descriptor,Descriptor>>& matches,
		const DescriptorDimentionSettings& settings
	);

	~HoughFeatureExtractor();
};

