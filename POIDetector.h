#pragma once
#include "Image.h"
#include "Point.h"
#include <vector>
#include <functional>
enum POISearchMethod{Moravec, Harris};
class POIDetector
{
public:
	POIDetector(POISearchMethod searchType);
	~POIDetector();
	vector<Point> FindPoints(Matrix2D& image,bool suppressNonMaximum=false,int leftPointCount=200);
protected:	
	double HalfWindowSize() { return windowSize / 2;}	
	vector<Point> ChoosePeaks(Matrix2D& contrastMatrix);
private:
	double threshold = 0;
	double const windowSize = 7;
	double const localWindowSize = 7;
	double const EqualityFactor = 1.1;
	std::function<Matrix2D(Matrix2D & image)> operatorValuesProcessor;
	bool CheckPointSuits(const Matrix2D& image,int x, int y);
	vector<Point> SuppressNonMaximum(Matrix2D& heatMap, vector<Point> foundPoints, int leftPointsCount);
	Matrix2D BuildMoravecOperatorValues(Matrix2D& image);
	Matrix2D BuildHarrisOperatorValues(Matrix2D& image);
};

