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
	auto rgbBytes = new uchar[qImage.width()*qImage.height()];
	for (int y = 0; y < qImage.height(); y++)
	{
		for (int x = 0; x < qImage.width(); x++)
		{
			QRgb pixel = qImage.pixel(x, y);
			int b = pixel % 256;
			int g = pixel / 256 % 256;
			int r = pixel / 256 / 256 % 256;
			rgbBytes[qImage.width()*y + x] = (uchar)min(255.,
				0.213 * r +
				0.715 * g+
				0.072 * b);
		}
	}return unique_ptr<Image>(new Image(rgbBytes,qImage.width(),qImage.height(),1));
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

void DrawPoints(const vector<Point>& points,QImage& qImage) 
{
	QPainter painter(&qImage);
	for each(auto point in points)
	{
		auto color = QColor(abs(rand()) % 256, abs(rand()) % 256, abs(rand()) % 256);		
		painter.setPen(color);
		painter.drawEllipse(
			point.x,
			point.y,
			2,
			2);
		
	}
}

void DrawBlobs(const vector<Blob>& blobs, QImage& qImage)
{
	QPainter painter(&qImage);
	for each(auto blob in blobs)
	{
		auto color = QColor(abs(rand()) % 256, abs(rand()) % 256, abs(rand()) % 256);
		painter.setPen(color);
		int r = blob.r;
		painter.drawEllipse(
			blob.x-r,
			blob.y-r,
			r*2,
			r*2);

	}
}

void DrawPoints(const vector<pair<Point, Point>>& matches, QImage& qImage,int width)
{
	QPainter painter(&qImage);
	for each(auto match in matches)
	{
		auto color = QColor(abs(rand()) % 256, abs(rand()) % 256, abs(rand()) % 256);		
		painter.fillRect(
			match.first.x-2,
			match.first.y-2,
			4,
			4,
			color);
		painter.fillRect(
			match.second.x+width-2,
			match.second.y-2,
			4,
			4,
			color);
		painter.setPen(color);
		painter.drawLine(
			match.first.x,
			match.first.y,
			match.second.x + width,
			match.second.y);
	}
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
	DrawPoints(matches, qImage,secondImageXShift);
	return qImage;
}

QImage PlatformImageUtils::DrawImage(const Matrix2D & image1, vector<Blob> blobs)
{
	Image image(image1);
	QImage qImage(image.Width(), image.Height(), QImage::Format::Format_RGB32);

	for (int y = 0; y < image.Height(); y++)
	{
		for (int x = 0; x < image.Width(); x++)
		{
			int color = (int)(image.PixelAt(x, y));
			qImage.setPixel(x, y, qRgb(color, color, color));
		}
	}
	DrawBlobs(blobs, qImage);
	return qImage;
}

QImage PlatformImageUtils::DrawImage(const Matrix2D & image1, vector<Point> points)
{
	auto resultImage = Matrix2D(image1.Width(), image1.Height());
	for (int y = 0; y < image1.Height(); y++)
	{
		for (int x = 0; x < image1.Width(); x++)
		{
			resultImage.SetElementAt(x, y, image1.At(x, y));
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
	DrawPoints(points, qImage);
	return qImage;
}
