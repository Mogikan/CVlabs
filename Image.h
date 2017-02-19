#pragma once

#include <stdlib.h>
#include <memory>
typedef unsigned char uchar;
using namespace std;
class Image
{
public:
	Image(uchar* rgbImageBytes,int width,int height,int bytesPerPixel);
	~Image();
	int GetWidth() const;
	int GetHeight() const;
	int GetBytesPerPixel() const;
	int GetTotalBytes() const;
	uchar* GetRawData();
private:
	unique_ptr<uchar[]> imageBytes;
	int width;
	int height;
	int bytesPerPixel;
};

