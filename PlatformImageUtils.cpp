#include "PlatformImageUtils.h"


PlatformImageUtils::PlatformImageUtils()
{
}

QImage PlatformImageUtils::QImageFromInternalImage(const Image& image)
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
	return qImage;
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

void DrawBlobs(const vector<BlobInfo>& blobs, QImage& qImage)
{
	QPainter painter(&qImage);
	for each(auto blob in blobs)
	{
		auto color = QColor(abs(rand()) % 256, abs(rand()) % 256, abs(rand()) % 256);
		painter.setPen(color);
		int r = blob.effectiveSigma * sqrt(2);
		painter.drawEllipse(
			blob.point.x * pow(2,blob.octave)-r,
			blob.point.y * pow(2, blob.octave)-r,
			r*2,
			r*2);

	}
}

void PlatformImageUtils::DrawDescriptors(const vector<pair<Descriptor, Descriptor>>& matches, QImage& qImage, int width)
{
	QPainter painter(&qImage);
	for (auto& descriptor : matches)
	{
		auto color = QColor(abs(rand()) % 256, abs(rand()) % 256, abs(rand()) % 256);
		painter.setPen(color);
		auto& point1 = descriptor.first.GetPoint();
		auto r1 = descriptor.first.Sigma() * sqrt(2);
		painter.drawEllipse(
			point1.x - r1,
			point1.y - r1,
			r1*2,
			r1*2);
		auto& point2 = descriptor.second.GetPoint();
		auto r2 = descriptor.second.Sigma() * sqrt(2);
		painter.drawEllipse(
			point2.x - r2+width,
			point2.y - r2,
			r2*2,
			r2*2);
		painter.drawLine(
			point1.x,
			point1.y,
			point2.x + width,
			point2.y);
	}
}

void DrawPoints(const vector<pair<Point, Point>>& matches, QImage& qImage,int width)
{
	QPainter painter(&qImage);
	for(auto& match: matches)
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
	vector<pair<Descriptor, Descriptor>> matches)
{
	int secondImageXShift = image1.Width();
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
	DrawDescriptors(matches, qImage,secondImageXShift);
	return qImage;
}

QImage PlatformImageUtils::DrawImage(const Matrix2D & image1, vector<BlobInfo> blobs)
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

void PlatformImageUtils::DrawObjectBounds(QImage & image, Size objectSize, TransformationMetaInfo metaInfo)
{
	QPainter painter(&image);
	painter.save();
	painter.translate(metaInfo.x,metaInfo.y);
	painter.scale(metaInfo.scale,metaInfo.scale);
	double rotation = metaInfo.rotation / M_PI * 180;
	painter.rotate(rotation);
	painter.drawRect(-objectSize.w / 2, -objectSize.h / 2, objectSize.w, objectSize.h);
	painter.restore();
}

void PlatformImageUtils::DrawObjectBounds(QImage & image, Size objectSize, const Matrix2D& t)
{
	QPainter painter(&image);
	painter.save();
	auto color = QColor(abs(rand()) % 256, abs(rand()) % 256, abs(rand()) % 256);
	painter.setPen(color);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);
	QTransform htransform(t.At(0), t.At(1), t.At(2), t.At(3), t.At(4), t.At(5), t.At(6), t.At(7), t.At(8));
	painter.setTransform(htransform.transposed());
	painter.drawRect(0, 0, objectSize.w, objectSize.h);
	painter.restore();
}

void PlatformImageUtils::DrawLines(QImage& image, vector<vector<Point>> points)
{
	QPainter painter(&image);
	for (int i = 0; i < points.size(); i++)
	{
		auto& line = points[i];
		auto color = QColor(abs(rand()) % 256, abs(rand()) % 256, abs(rand()) % 256);
		painter.setPen(color);
		
		for (int j = 0; j < line.size()-1; j++)
		{
			auto& point1 = line[j];
			auto& point2 = line[j + 1];
			painter.drawLine(point1.x, point1.y, point2.x, point2.y);
		}
	}
}

void PlatformImageUtils::DrawEllipses(QImage & image, vector<EllipseDescriptor> ellipses)
{
	QPainter painter(&image);
	for (int i = 0; i < ellipses.size(); i++)
	{
		auto& ellipse = ellipses[i];
		auto color = QColor(abs(rand()) % 256, abs(rand()) % 256, abs(rand()) % 256);
		QPen pen(color);
		pen.setWidth(3);
		painter.setPen(pen);		
		painter.save();
		painter.translate(ellipse.x, ellipse.y);
		painter.rotate((ellipse.fi)/M_PI*180);
		painter.drawEllipse(QPointF(0,0), ellipse.a, ellipse.b);
		painter.restore();
	}
}

void PlatformImageUtils::DrawCircles(QImage & image, vector<CircleDescriptor> circles)
{
	QPainter painter(&image);
	for (int i = 0; i < circles.size(); i++)
	{
		auto& circle = circles[i];
		auto color = QColor(abs(rand()) % 256, abs(rand()) % 256, abs(rand()) % 256);
		QPen pen(color);
		pen.setWidth(3);
		painter.setPen(pen);
		painter.drawEllipse(circle.x-circle.r, circle.y-circle.r, circle.r*2, circle.r*2);		
	}
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

QImage PlatformImageUtils::CombineImages(const QImage& image1,const QImage& image2,const Matrix2D& t)
{
	QImage qImage(image1.width()+image2.width(), (image1.height() + image2.height())/2*1.4, QImage::Format::Format_RGB32);
	QPainter painter(&qImage);
	painter.drawImage(0, 0, image1);	
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);
	QTransform htransform(t.At(0), t.At(1),t.At(2), t.At(3), t.At(4), t.At(5), t.At(6), t.At(7), t.At(8));	
	painter.setTransform(htransform.transposed());
	painter.drawImage(0, 0,image2);
	return qImage;
}

