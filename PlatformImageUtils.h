#pragma once
#include <stdlib.h>
#include <memory>
#include <QtGui>
#include "Image.h"
using namespace std;
class PlatformImageUtils
{
public:
	PlatformImageUtils();
	static QImage QImageFromInternalImage(const Image& image);
	static unique_ptr<Image> LoadInternalImage(QString qImageFileName);	
	static unique_ptr<Image> ConvertQImageToInternalImage(QImage qImage);	
	static void SaveImage(Image& image, QString filePath);
	static QImage LoadQImageFromFile(QString filePath);
private:
	
};
