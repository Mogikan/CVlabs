#pragma once
#include "Layer.h"
using namespace std;
class Octave
{
public:
	Octave(unique_ptr<Matrix2D> firstImage, double sigma,int layerInOctave);
	Octave(const Octave& octave);
	void AddLayer(unique_ptr<Layer> layer);
	int GetLayersCount();
	const Layer& LayerAt(size_t index);
private:
	std::vector<unique_ptr<Layer>> layers;
	int layersCount=0;
};

