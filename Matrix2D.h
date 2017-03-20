#pragma once
#include <memory>
#include <vector>
enum BorderMode { wrap, mirror, extend, zero };
using namespace std;
class Matrix2D
{
public:
	Matrix2D(double values[], int width, int height);
	Matrix2D(vector<double> imagePixels, int width, int height);
	Matrix2D(int width, int height);	
	Matrix2D(const Matrix2D& matrix);
	double GetIntensity(int x, int y, BorderMode borderHandlingMode = BorderMode::extend) const;
	double At(int x, int y) const;
	double At(int plainIndex) const;
	void SetElementAt(int x, int y, double value);
	void SetElementAt(int plainIndex, double value);
	vector<double> ExtractData() const;
	virtual void Normalize();
	int Width() const;
	int Height() const;
	int TotalElements();
	~Matrix2D();
protected:
	std::vector<double> elements;
private:
	int width;
	int height;	
};

