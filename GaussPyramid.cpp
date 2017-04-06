#include "GaussPyramid.h"
#include "ImageFramework.h"
#include "PlatformImageUtils.h"
GaussPyramid::GaussPyramid(
	const Matrix2D& originalImage, 
	int octaveCount, 
	int layersInOctave, 
	double sigma0, 
	double sigmaA)
{
	this->sigma0 = sigma0;
	this->sigmaA = sigmaA;
	double firstSigmaDelta = sqrt(sigma0*sigma0 - sigmaA*sigmaA);	
	auto firstImage = ImageFramework::ApplyGaussSmooth(originalImage, firstSigmaDelta);
	BuildOctaves(move(firstImage),octaveCount, layersInOctave);
}

int GaussPyramid::OctavesCount()
{
	return octaves.size();
}

const Octave & GaussPyramid::OctaveAt(int index)
{
	return *octaves.at(index);
}

void GaussPyramid::AddOctave(unique_ptr<Octave> octave)
{
	octaves.push_back(move(octave));
}

double GaussPyramid::L(int x, int y, double sigma)
{
	double pyramidSigma = sigma / sigma0;
	auto level = (int)log2(pyramidSigma);
	auto nearestOctave = OctaveAt(level);
	int effectiveX = (int)(x / pow(2, level));
	int effectiveY = (int)(y / pow(2, level));
	double sigmaRest = pyramidSigma / pow(2, level);
	auto layersInOctave = nearestOctave.LayersCount();
	double scaleInterval = pow(2, 1. / layersInOctave);
	int leftLayer = log(sigmaRest)/log(scaleInterval);
	int rightLayer = leftLayer+1;
	double leftSigma = nearestOctave.LayerAt(leftLayer).Sigma();
	double rightSigma = nearestOctave.LayerAt(rightLayer).Sigma();
	int nearestLayer;
	nearestLayer = (abs(sigmaRest - leftSigma) < abs(sigmaRest - rightSigma))?
		leftLayer:rightLayer;	
	return nearestOctave.LayerAt(nearestLayer).GetImage().PixelAt(effectiveX, effectiveY);
}

bool IsLocalExtremum(
	int x, 
	int y,
	const Matrix2D& previousDOG, 
	const Matrix2D& currentDOG, 
	const Matrix2D& nextDOG, 
	double minDiffValue,
	double maxDiffValue)
{
	bool isMax = true;
	bool isMin = true;	
	double currentValue = currentDOG.GetIntensity(x, y);	
	if (abs(currentValue) < 0.04)
	{
		return false;
	}
	for (int dy = -1; dy <= 1; dy++)
	{
		for (int dx = -1; dx <= 1; dx++)
		{
			//if (dx == 0 && dy == 0)
			//{
			//	continue;
			//}			
			double vicinityPointValue0 = previousDOG.GetIntensity(x + dx, y + dy);
			double vicinityPointValue1 = currentDOG.GetIntensity(x + dx, y + dy);
			double vicinityPointValue2 = nextDOG.GetIntensity(x + dx, y + dy);
			if (!(currentValue>vicinityPointValue0  
				&& currentValue>vicinityPointValue2 
				&& (currentValue>vicinityPointValue1 
					||(dx==0&&dy==0))))				
			{
				isMax = false;
			}
			if (!(currentValue<vicinityPointValue0  
				&& currentValue<vicinityPointValue2 
				&& (currentValue<vicinityPointValue1 
					||(dx == 0 && dy == 0))
				))
			{
				isMin = false;
			}
			if (isMax==false&&isMin==false) 
			{
				return false;
			}
		}
	}
	return true;	
}

vector<Blob> GaussPyramid::FindBlobs()
{	
	vector<Blob> result;
	for (int i = 0;i < octaves.size();i++)
	{
		auto& octave = OctaveAt(i);		
		auto diffs = octave.ComputeDiffs();
		for (int j = 1; j < diffs.size() - 1; j++)
		{
			auto& previousDiff = diffs[j - 1];
			auto& currentDiff = diffs[j];
			double maxDiffValue;
			double minDiffValue;
			tie(minDiffValue, maxDiffValue) = currentDiff.first->MinMax();
			auto& nextDiff = diffs[j + 1];
			//PlatformImageUtils::SaveImage(Image(*(currentDiff.first)),
			//	"C:\\Pyramid\\Octave_" + QString::number(i) + "_Layer_" + QString::number(j) + ".png");		

			for (int y = 0; y < octave.ImageHeight(); y++)
			{
				for (int x = 0; x < octave.ImageWidth(); x++)
				{
					if (IsLocalExtremum(x, y, *previousDiff.first, *currentDiff.first,*nextDiff.first,minDiffValue,maxDiffValue))
					{
						double harris = POIDetector::HarrisOperatorValueAt(x, y, octave.LayerAt(j).ImageD());
						if (harris > POIDetector::HarrisThreshold/2)
						{
							result.push_back(Blob(x*pow(2, i), y*pow(2, i), currentDiff.second*sqrt(2)));
						}
					}
				}
			}
		}		
	}
	return result;
}

void GaussPyramid::BuildOctaves(unique_ptr<Matrix2D> firstImage,int octaveCount, int layersInOctave)
{	
	auto runningImage = move(firstImage);	
	for (int i = 0; i < octaveCount; i++)
	{
		AddOctave(make_unique<Octave>(move(runningImage), layersInOctave, sigma0, i));
		auto& octave = OctaveAt(OctavesCount()-1);		
		auto lastOctaveImage = octave.LayerAt(octave.LayersCount()-1).ImageD();
		runningImage = ImageFramework::DownscaleImageTwice(lastOctaveImage);
	}
}


