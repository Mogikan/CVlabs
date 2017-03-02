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

void GaussPyramid::BuildOctaves(unique_ptr<Matrix2D> firstImage,int octaveCount, int layersInOctave)
{
	auto runningImage = move(firstImage);
	double runningSigma = sigma0;
	for (int i = 0; i < octaveCount; i++)
	{	

		AddOctave(make_unique<Octave>(move(runningImage), layersInOctave, runningSigma, i));
		auto octave = OctaveAt(OctavesCount()-1);
		//runningSigma *= 2;
		auto lastOctaveImage = octave.LayerAt(octave.LayersCount() - 1).Image();
		runningImage = ImageFramework::DownscaleImageTwice(lastOctaveImage);
	}
}
