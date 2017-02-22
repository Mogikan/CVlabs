#pragma once
#include <memory>
using namespace std;
class Matrix2D
{
public:
	Matrix2D(double values[], int width, int height);
	Matrix2D(unique_ptr<double[]> imagePixels,int width, int height);
	double GetElementAt(int x, int y);
	void SetElementAt(int x, int y, double value);
	unique_ptr<double[]> ExtractData();
	const unique_ptr<double[]>& ReadData();
	int GetWidth();
	int GetHeight();
	int GetTotalElements();
	~Matrix2D();
private:
	int width;
	int height;
	unique_ptr<double[]> elements;
};

