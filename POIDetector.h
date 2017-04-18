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
	vector<Point> FindPoints(const Matrix2D& image,bool suppressNonMaximum=false,int leftPointCount=200);
	static double HarrisOperatorValueAt(int x, int y, const Matrix2D& image,int regionWindowSize);
	static double const HarrisThreshold;
protected:	
	static double HalfWindowSize() { return windowSize / 2;}	
	vector<Point> ChoosePeaks(const Matrix2D& contrastMatrix);	
private:
	static double const windowSize;	
	double threshold = 0;	
	double const localWindowSize = 7;
	double const EqualityFactor = 1.1;
	std::function<Matrix2D(const Matrix2D & image)> operatorValuesProcessor;
	bool CheckPointSuits(const Matrix2D& image,int x, int y);
	vector<Point> SuppressNonMaximum(const Matrix2D& heatMap, vector<Point> foundPoints, int leftPointsCount);
	Matrix2D BuildMoravecOperatorValues(const Matrix2D& image);
	Matrix2D BuildHarrisOperatorValues(const Matrix2D& image);
};

