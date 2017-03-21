#include "GaussPyramid.h"
#include "ImageFramework.h"

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

void GaussPyramid::BuildOctaves(unique_ptr<Matrix2D> firstImage,int octaveCount, int layersInOctave)
{

	auto runningImage = move(firstImage);	
	for (int i = 0; i < octaveCount; i++)
	{	

		AddOctave(make_unique<Octave>(move(runningImage), layersInOctave, sigma0, i));
		auto octave = OctaveAt(OctavesCount()-1);		
		auto lastOctaveImage = octave.LayerAt(octave.ImageCount() - 1).ImageD();
		runningImage = ImageFramework::DownscaleImageTwice(lastOctaveImage);
	}
}
