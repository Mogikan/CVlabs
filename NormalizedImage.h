#pragma once
#include <memory>
using namespace std;
class NormalizedImage
{
public:
	NormalizedImage(unique_ptr<double[]> imagePixels,int width, int height);
	double GetElementAt(int x, int y);
	int GetWidth();
	int GetHeight();
	~NormalizedImage();
private:
	int width;
	int height;
	unique_ptr<double[]> imagePixels;
};

