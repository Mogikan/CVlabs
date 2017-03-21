#include "POIDetector.h"
#include "ImageFramework.h"
#include "MathHelper.h"

POIDetector::POIDetector(POISearchMethod searchType)
{
	switch (searchType)
	{
	case POISearchMethod::Harris:
		threshold = 0.05;
		operatorValuesProcessor = [&](const Matrix2D & image) {return BuildHarrisOperatorValues(image); };
		break;
	case POISearchMethod::Moravec:
		threshold = 0.001;
		operatorValuesProcessor = [&](const Matrix2D & image) {return BuildMoravecOperatorValues(image); };
		break;
	default:
		break;
	}
}

POIDetector::~POIDetector()
{
}

vector<Point> POIDetector::FindPoints(const Matrix2D & image, bool suppressNonMaximum,int leftPointCount)
{
	auto smothedImage = ImageFramework::ApplyGaussSmooth(image, 1.5);
	auto specialPointsOperatorValues = operatorValuesProcessor(*smothedImage);
	auto foundPoints = ChoosePeaks(specialPointsOperatorValues);
	if (suppressNonMaximum)
	{
		auto filteredPoints = SuppressNonMaximum(image, foundPoints, leftPointCount);
		return filteredPoints;
	}
	else 
	{
		return foundPoints;
	}

}

Matrix2D POIDetector::BuildMoravecOperatorValues(const Matrix2D & image)
{
	double const shift = 1;
	auto minContrastMatrix = Matrix2D(image.Width(), image.Height());
	for (int y = 0; y < image.Height(); y++)
	{
		for (int x = 0; x < image.Width(); x++)
		{
			double minContrast = DBL_MAX;
			for (int shiftDirectionX = -1; shiftDirectionX <= 1; shiftDirectionX++)
			{
				for (int shiftDirectionY = -1; shiftDirectionY <= 1; shiftDirectionY++)
				{
					if (shiftDirectionX == 0 && shiftDirectionY == 0)
						continue;
					int shiftedX = x + shift*shiftDirectionX;
					int shiftedY = y + shift*shiftDirectionY;
					int windowHalfSize = HalfWindowSize();
					double currentContrast = 0;
					for (int windowY = -windowHalfSize; windowY <= windowHalfSize; windowY++)
					{
						for (int windowX = -windowHalfSize; windowX <= windowHalfSize; windowX++)
						{
							int pixelX = x + windowX;
							int pixelY = y + windowY;
							int shiftedPixelX = shiftedX + windowX;
							int shiftedPixelY = shiftedY + windowY;
							double difference = 
								image.GetIntensity(pixelX, pixelY) 
								- image.GetIntensity(shiftedPixelX, shiftedPixelY);
							currentContrast += difference*difference;
						}
					}
					if (currentContrast < minContrast)
						minContrast = currentContrast;
				}
			}
			minContrastMatrix.SetElementAt(x, y, minContrast);
		}
	}
	//PlatformImageUtils::SaveImage(Image(*minContrastMatrix), "C:\\moravecContrast.png");
	return minContrastMatrix;
}
Matrix2D POIDetector::BuildHarrisOperatorValues(const Matrix2D & image)
{
	auto dx = ImageFramework::ApplySobelX(image);
	auto dy = ImageFramework::ApplySobelY(image);
	auto minContrastMatrix = Matrix2D(image.Width(), image.Height());
	for (int y = 0; y < image.Height(); y++)
	{
		for (int x = 0; x < image.Width(); x++)
		{
			Matrix2D H(2, 2);
			double a = 0;
			double b = 0;
			double c = 0;
			for (int shiftY = -HalfWindowSize(); shiftY <= HalfWindowSize(); shiftY++)
			{
				for (int shiftX = -HalfWindowSize(); shiftX <= HalfWindowSize(); shiftX++)
				{
					double Ix = dx->GetIntensity(x + shiftX, y + shiftY);
					double Iy = dy->GetIntensity(x + shiftX, y + shiftY);
					a += Ix*Ix;
					b += Ix*Iy;
					c += Iy*Iy;
				}
			}
			H.SetElementAt(0, 0, a);
			H.SetElementAt(0, 1, b);
			H.SetElementAt(1, 0, b);
			H.SetElementAt(1, 1, c);
			auto eigenvalues = MathHelper::Eigenvalues(H);
			auto lambdaMin = min(eigenvalues.first, eigenvalues.second);
			minContrastMatrix.SetElementAt(x, y, lambdaMin);
		}
	}
	//PlatformImageUtils::SaveImage(Image(*minContrastMatrix), "C:\\harrisLambdas.png");
	return minContrastMatrix;
}

vector<Point> POIDetector::ChoosePeaks(const Matrix2D & contrastMatrix)
{
	vector<Point> result;
	for (int y = 0; y < contrastMatrix.Height(); y++)
	{
		for (int x = 0; x < contrastMatrix.Width(); x++)
		{
			
			auto currentIntensity = contrastMatrix.GetIntensity(x, y);
			if (currentIntensity > threshold) 
			{
				if (CheckPointSuits(contrastMatrix, x, y))
				{
					result.push_back(Point(x, y));
				}
			}
		}
	}
	return result;
}

bool POIDetector::CheckPointSuits(const Matrix2D& image, int x, int y)
{
	int localMaxWindowHalf = localWindowSize / 2;
	double currentIntensity = image.GetIntensity(x,y);
	for (int dy = -localMaxWindowHalf; dy <= localMaxWindowHalf; dy++)
	{
		for (int dx = -localMaxWindowHalf; dx <= localMaxWindowHalf; dx++)
		{
			if (dx == 0 && dy == 0)
				continue;
			if (currentIntensity <= image.GetIntensity(x + dx, y + dy))
			{
				return false;				
			}
		}		
	}
	return true;
}

double DistanceSQR(Point first, Point second)
{
	double diffX = first.x - second.x;
	double diffY = first.y - second.y;
	return diffX*diffX + diffY*diffY;
}

vector<Point> POIDetector::SuppressNonMaximum(
	const Matrix2D & operatorValues, 
	vector<Point> foundPoints,
	int leftPointsCount
)
{
	vector<Point> result = foundPoints;
	double r = 0;
	while (result.size()>leftPointsCount)
	{
		for (int point1Index = 0; point1Index < result.size(); point1Index++)
		{
			for (int point2Index = point1Index+1; point2Index < result.size(); point2Index++)
			{
				if (point2Index == point1Index)
				{
					continue;
				}
				auto firstPoint = result[point1Index];
				auto secondPoint = result[point2Index];
				if (DistanceSQR(firstPoint, secondPoint) < r*r)
				{
					if (operatorValues.At(firstPoint.x, firstPoint.y)> 
						EqualityFactor * operatorValues.At(secondPoint.x, secondPoint.y))
					{
						result.erase(result.begin() + point2Index);
						point2Index--;
					}
					else 
					{
						result.erase(result.begin()+ point1Index);
						point1Index = (point1Index>0)?point1Index--:0;
						point2Index = point1Index + 1;
					}					
				}
			}
		}
		r += 0.9;
	}
	return result;
}
