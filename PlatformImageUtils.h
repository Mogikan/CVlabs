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
	static unique_ptr<Image> LoadInternalImage(QString qImageFileName);
	static void SaveImage(const unique_ptr<Image>& image, QString filePath);
private:
	static unique_ptr<Image> ConvertQImageToInternalImage(QImage qImage);
	static QImage LoadQImageFromFile(QString filePath);
};
