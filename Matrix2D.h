#pragma once
#include <memory>
#include <vector>
using namespace std;
class Matrix2D
{
public:
	Matrix2D(double values[], int width, int height);
	Matrix2D(vector<double> imagePixels, int width, int height);
	Matrix2D(int width, int height);	
	Matrix2D(const Matrix2D& matrix);
	double GetElementAt(int x, int y);
	double GetElementAt(int plainIndex);
	void SetElementAt(int x, int y, double value);
	void SetElementAt(int plainIndex, double value);
	vector<double> ExtractData() const;
	int Width() const;
	int Height() const;
	int TotalElements();
	~Matrix2D();
private:
	int width;
	int height;
	std::vector<double> elements;
};

