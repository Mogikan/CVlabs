#pragma once
#include <stdlib.h>
#include <memory>
#include <QtGui>
#include "Image.h"
#include "Point.h"
#include "BlobInfo.h"
using namespace std;
class PlatformImageUtils
{
public:
	PlatformImageUtils();
	static QImage QImageFromInternalImage(const Image& image);
	static unique_ptr<Image> LoadInternalImage(QString qImageFileName);	
	static unique_ptr<Image> ConvertQImageToInternalImage(QImage qImage);	
	static void SaveImage(Image& image, QString filePath);
	static QImage DrawImage(
		const Matrix2D& image1, 
		const Matrix2D& image2, 
		vector<pair<Point, Point>> matches, 
		int secondImageXShift);
	static QImage DrawImage(const Matrix2D& image1, vector<Point> points);
	static QImage DrawImage(const Matrix2D& image, vector<BlobInfo> blobs);
	static QImage LoadQImageFromFile(QString filePath);
private:
	
};
