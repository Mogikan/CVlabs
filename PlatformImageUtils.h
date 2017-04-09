#pragma once
#include <stdlib.h>
#include <memory>
#include <QtGui>
#include "Image.h"
#include "Point.h"
#include "BlobInfo.h"
#include "Descriptor.h"
using namespace std;
class PlatformImageUtils
{
public:
	PlatformImageUtils();
	static QImage QImageFromInternalImage(const Image& image);
	static unique_ptr<Image> LoadInternalImage(QString qImageFileName);	
	static unique_ptr<Image> ConvertQImageToInternalImage(QImage qImage);	
	static void SaveImage(Image& image, QString filePath);
	static void DrawDescriptors(
		const vector<pair<Descriptor, Descriptor>>& matches,
		QImage& qImage,
		int width);
	static QImage DrawImage(
		const Matrix2D& image1, 
		const Matrix2D& image2, 
		vector<pair<Descriptor, Descriptor>> matches, 
		int secondImageXShift);
	static QImage DrawImage(const Matrix2D& image1, vector<Point> points);
	static QImage DrawImage(const Matrix2D& image, vector<BlobInfo> blobs);
	static QImage CombineImages(const QImage& image1, const QImage& image2, const Matrix2D& t);
	static QImage LoadQImageFromFile(QString filePath);
private:
	
};
