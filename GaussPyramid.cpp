#include "GaussPyramid.h"
#include "ImageFramework.h"

GaussPyramid::GaussPyramid(
	Matrix2D& originalImage, 
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
	return octavesCount;
}

const Octave & GaussPyramid::OctaveAt(int index)
{
	return *octaves[index];
}

void GaussPyramid::AddOctave(unique_ptr<Octave> octave)
{
	octaves.push_back(move(octave));
	octavesCount++;
}

double GaussPyramid::L(int x, int y, double sigma)
{
	auto level = (int)log2(sigma);
	auto octave = OctaveAt(level);
	int effectiveX = x / pow(2, level);
	int effectiveY = y / pow(2, level);
	double sigmaRest = sigma / pow(2, level);
	auto layersInOctave = octave.LayersCount() - 1;
	double scaleInterval = pow(2, 1. / layersInOctave);
	double levelSigma = 1;
	double nextSigma = levelSigma*scaleInterval;
	int desiredLevel = 0;
	while (!(abs(sigmaRest - levelSigma) < abs(sigmaRest - nextSigma)))
	{		
		desiredLevel++;
		levelSigma = nextSigma;
		nextSigma = levelSigma * scaleInterval;
	}
	return octave.LayerAt(desiredLevel).Image().PixelAt(effectiveX, effectiveY);
}

void GaussPyramid::BuildOctaves(unique_ptr<Matrix2D> firstImage,int octaveCount, int layersInOctave)
{
	auto runningImage = move(firstImage);	
	for (int i = 0; i < octaveCount; i++)
	{	

		AddOctave(make_unique<Octave>(move(runningImage), layersInOctave, sigma0, i));
		auto octave = OctaveAt(OctavesCount()-1);		
		auto lastOctaveImage = octave.LayerAt(octave.LayersCount() - 1).ImageD();
		runningImage = ImageFramework::DownscaleImageTwice(lastOctaveImage);
	}
}
