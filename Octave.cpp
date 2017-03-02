#include "Octave.h"
#include "ImageFramework.h"

Octave::Octave(unique_ptr<Matrix2D> firstImage, int layersInOctave, double sigma, int depth)
{
	AddLayer(move(make_unique<Layer>(move(firstImage), sigma, depth)));
	double scaleInterval = pow(2, 1./layersInOctave);
	double runningSigma = sigma;
	for (int i = 0; i < layersInOctave; i++)
	{
		double nextSigma = runningSigma * scaleInterval;
		double convolutionSigma = sqrt(nextSigma*nextSigma - runningSigma*runningSigma);
		runningSigma = nextSigma;
		auto runningImage = LayerAt(LayersCount() - 1).Image();
		auto nextImage = ImageFramework::ApplyGaussSmooth(runningImage,convolutionSigma);
		AddLayer(move(make_unique<Layer>(move(nextImage), runningSigma,depth)));
	}	
}

Octave::Octave(const Octave& octave)
{
	layers = vector<unique_ptr<Layer>>();
	for (const auto& e : octave.layers)
		AddLayer(make_unique<Layer>(*e));
}

void Octave::AddLayer(unique_ptr<Layer> layer)
{
	layers.push_back(move(layer));
	layersCount++;
}


int Octave::LayersCount()
{	
	return layersCount;
}

const Layer& Octave::LayerAt(int index)
{
	return *layers.at(index);
}
