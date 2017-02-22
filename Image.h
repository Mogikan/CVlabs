#pragma once

#include <stdlib.h>
#include <memory>
#include <algorithm>
#include <Matrix2D.h>
typedef unsigned char uchar;
using namespace std;
class Image
{
public:
	Image(uchar* rgbImageBytes,int width,int height,int bytesPerPixel);
	Image(const unique_ptr<Matrix2D>& imageMatrix);
	Image(const unique_ptr<double[]>& normalizedPixels, int width, int height);
	~Image();
	int GetWidth() const;
	int GetHeight() const;
	int GetBytesPerPixel() const;
	int GetTotalBytes() const;	
	unique_ptr<double[]> GetDoubleData();
	unique_ptr<double[]> GetNormilizedDoubleData();
	unique_ptr<Matrix2D> GetDoubleMatrix();
	uchar* GetRawData();
	void NormalizeImage();
private:
	
	unique_ptr<uchar[]> imageBytes;
	int width;
	int height;
	int bytesPerPixel;
};

