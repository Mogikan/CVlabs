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
	vector<Point> FindPoints(Matrix2D& image,bool suppressNonMaximum=false,int leftPointCount=100);
protected:	
	double HalfWindowSize() { return windowSize / 2;}	
	vector<Point> ChoosePeaks(Matrix2D& contrastMatrix);
private:
	double threshold = 0;
	double const windowSize = 7;
	double const localWindowSize = 7;
	std::function<unique_ptr<Matrix2D>(Matrix2D & image)> heatMapImplementation;
	bool CheckPointSuits(const Matrix2D& image,int x, int y);
	vector<Point> SuppressNonMaximum(Matrix2D& heatMap, vector<Point> foundPoints, int leftPointsCount);
	unique_ptr<Matrix2D> BuildHeatMapMoravec(Matrix2D& image);
	unique_ptr<Matrix2D> BuildHeatMapHarris(Matrix2D& image);
};

