#pragma once
#include "Layer.h"
using namespace std;
class Octave
{
public:
	Octave(unique_ptr<Matrix2D> firstImage,int layerInOctave, double effectiveSigma,int depth);
	Octave(const Octave& octave);
	void AddLayer(unique_ptr<Layer> layer);
	int ImageCount() const;
	int LayersCount() const;
	const Layer& LayerAt(int index);
private:
	std::vector<unique_ptr<Layer>> layers;
	int imageCount=0;
	int layersCount = 0;
};

