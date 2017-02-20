#pragma once
#include <memory>
using namespace std;
class Matrix2D
{
public:
	Matrix2D(unique_ptr<double[]> imagePixels,int width, int height);
	double GetElementAt(int x, int y);
	void SetElementAt(int x, int y, double value);
	unique_ptr<double[]> ExtractData();
	int GetWidth();
	int GetHeight();
	~Matrix2D();
private:
	int width;
	int height;
	unique_ptr<double[]> elements;
};

