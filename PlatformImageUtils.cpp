#include "PlatformImageUtils.h"


PlatformImageUtils::PlatformImageUtils()
{
}

QImage PlatformImageUtils::QImageFromInternalImage(const Image& image)
{
	return QImage(image.GetRawData(),image.Width(),image.Height(), QImage::Format_Grayscale8);
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

QImage PlatformImageUtils::DrawImage(
	const Matrix2D& image1,
	const Matrix2D& image2, 
	vector<pair<Point, Point>> matches,
	int secondImageXShift)
{
	auto resultImage = Matrix2D(image1.Width() + image2.Width(), std::max(image1.Height(), image2.Height()));
	for (int y = 0; y < image1.Height(); y++)
	{
		for (int x = 0; x < image1.Width(); x++)
		{
			resultImage.SetElementAt(x, y, image1.At(x, y));
		}
	}
	for (int y = 0; y < image2.Height(); y++)
	{
		for (int x = 0; x < image2.Width(); x++)
		{
			resultImage.SetElementAt(x + image1.Width(), y, image2.At(x, y));
		}
	}
	Image image(resultImage);
	QImage qImage(image.Width(), image.Height(), QImage::Format::Format_RGB32);

	for (int y = 0; y < image.Height(); y++)
	{
		for (int x = 0; x < image.Width(); x++)
		{
			int color = (int)(image.PixelAt(x, y));
			qImage.setPixel(x, y, qRgb(color, color, color));
		}
	}
	for (int i = 0; i < matches.size(); i++)
	{
		QPainter painter(&qImage);
		for each(auto match in matches) 
		{
			painter.setPen(QColor(255,0,0));
			painter.drawLine(
				match.first.x, 
				match.first.y, 
				match.second.x+secondImageXShift,
				match.second.y);
		}
	}
	return qImage;
}
