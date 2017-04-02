#include "Octave.h"
#include "ImageFramework.h"

Octave::Octave(unique_ptr<Matrix2D> firstImage, int layersInOctave, double sigma, int depth)
{
	layersCount = layersInOctave;
	layers.push_back(move(make_unique<Layer>(move(firstImage), sigma, depth)));
	double scaleInterval = pow(2, 1./(layersInOctave));
	double runningSigma = sigma;
	for (int i = 1; i < layersInOctave+3; i++)
	{
		double nextSigma = runningSigma * scaleInterval;
		double convolutionSigma = sqrt(nextSigma*nextSigma - runningSigma*runningSigma);
		runningSigma = nextSigma;
		auto runningImage = LayerAt(ImageCount() - 1).ImageD();
		auto nextImage = ImageFramework::ApplyGaussSmooth(runningImage,convolutionSigma);
		layers.push_back(move(make_unique<Layer>(move(nextImage), runningSigma,depth)));
	}	
}

Octave::Octave(const Octave& octave)
{
	this->layersCount = octave.layersCount;	
	for (const auto& e : octave.layers)
		layers.push_back(make_unique<Layer>(*e));
}


int Octave::ImageCount() const
{	
	return layers.size();
}

int Octave::LayersCount() const
{
	return layersCount;
}

const Layer& Octave::LayerAt(int index) const
{
	return *layers.at(index);
}
