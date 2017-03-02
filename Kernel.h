#pragma once
#include <stdlib.h>
#include <memory>
#include <algorithm>
#include <Matrix2D.h>
#include <Point.h>
using namespace std;
class Kernel : public Matrix2D
{
public:	
	Kernel(double values[], int width, int height,Point applicationPoint);
	Kernel(vector<double> values, int width, int height,Point applicationPoint);
	Kernel(const Kernel& kernel);
	~Kernel();
	Point Center();
	static Kernel GetSobelX();
	static Kernel GetSobelY();

	static unique_ptr<Kernel> BuildGaussX(double sigma);
	static unique_ptr<Kernel> BuildGaussY(double sigma);
private:
	Point applicationPoint;
};