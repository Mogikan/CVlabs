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
	static unique_ptr<Image> ConvertQImageToInternalImage(QImage qImage);
	static QImage LoadQImageFromFile(QString filePath);
	static void SaveImage(shared_ptr<Image> image, QString filePath);
};
