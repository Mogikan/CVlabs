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
	Image(const Matrix2D& imageMatrix);
	Image(vector<double> normalizedPixels, int width, int height);
	~Image();
	int Width() const;
	int Height() const;
	uchar PixelAt(int x, int y) const;
	int BytesPerPixel() const;
	int GetTotalBytes() const;	
	vector<double> GetDoubleData();
	vector<double> GetNormilizedDoubleData();
	unique_ptr<Matrix2D> GetNormalizedMatrix();
	unique_ptr<Matrix2D> GetDoubleMatrix();
	uchar* GetRawData() const;
	void NormalizeImage();
private:	
	unique_ptr<uchar[]> imageBytes;
	int width;
	int height;
	int bytesPerPixel;	
};

