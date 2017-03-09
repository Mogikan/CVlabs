#pragma once
#include "Image.h"
#include "Point.h"
#include <vector>
class POIDetector
{
public:
	POIDetector();
	~POIDetector();
	vector<Point> FindPoints(Matrix2D& image,bool suppressNonMaximum=false,int leftPointCount=100);
protected:
	virtual unique_ptr<Matrix2D> BuildHeatMap(Matrix2D& image)= 0;

	virtual double Threshold() = 0;
	double HalfWindowSize() { return windowSize / 2;}	
	vector<Point> ChoosePeaks(Matrix2D& contrastMatrix);
private:
	double const windowSize = 7;
	double const localWindowSize = 7;
	bool CheckPointSuits(const Matrix2D& image,int x, int y);
	vector<Point> SuppressNonMaximum(Matrix2D& heatMap, vector<Point> foundPoints, int leftPointsCount);
};

