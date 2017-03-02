#include "PlatformImageUtils.h"


PlatformImageUtils::PlatformImageUtils()
{
}

QImage PlatformImageUtils::QImageFromInternalImage(const unique_ptr<Image>& image)
{
	return QImage(image->GetRawData(),image->Width(),image->Height(), QImage::Format_Grayscale8);
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

void PlatformImageUtils::SaveImage(Image& image, QString filePath)
{
	QImage qImage(image.Width(), image.Height(), QImage::Format::Format_RGB32);
	qImage.save(filePath);

	 for (int y = 0; y < image.Height(); y++)
	 { 
		 for (int x = 0; x < image.Width(); x++) 
		 { 
			 int color = (int)(image.PixelAt(x, y));   
			 qImage.setPixel(x, y, qRgb(color, color, color)); 
		 } 
	 }    
	 qImage.save(filePath); 
}
