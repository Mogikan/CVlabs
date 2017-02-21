#include "PlatformImageUtils.h"


PlatformImageUtils::PlatformImageUtils()
{
}

QImage PlatformImageUtils::QImageFromInternalImage(const unique_ptr<Image>& image)
{
	return QImage(image->GetRawData(),image->GetWidth(),image->GetHeight(), QImage::Format_Grayscale8);
}

unique_ptr<Image> PlatformImageUtils::LoadInternalImage(QString qImageFileName)
{
	return PlatformImageUtils::ConvertQImageToInternalImage(PlatformImageUtils::LoadQImageFromFile(qImageFileName));
}

unique_ptr<Image> PlatformImageUtils::ConvertQImageToInternalImage(QImage qImage)
{
	auto rgbImage = qImage.convertToFormat(QImage::Format_RGB888);
	int width = rgbImage.width();
	int height = rgbImage.height();
	int bytesPerPixel = rgbImage.bytesPerLine() / width;
	auto rgbBytes = new uchar[width*height*bytesPerPixel];
	
	copy((uchar*)rgbImage.bits(), ((uchar*)rgbImage.bits()) + rgbImage.byteCount(),rgbBytes);
	return unique_ptr<Image>(new Image(rgbBytes,width,height,bytesPerPixel));
}

QImage PlatformImageUtils::LoadQImageFromFile(QString fileName)
{
	return QImage(fileName);	
}

void PlatformImageUtils::SaveImage(const unique_ptr<Image>& image, QString filePath)
{
	auto imageData = image->GetRawData();
	QImage qImage(imageData, image->GetWidth(), image->GetHeight(), QImage::Format::Format_Grayscale8);
	qImage.save(filePath);
}
