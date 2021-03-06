#pragma once
#include "Layer.h"
using namespace std;
class Octave
{
public:
	Octave(unique_ptr<Matrix2D> firstImage,int layerInOctave, double effectiveSigma,int depth);
	Octave(const Octave& octave);	
	int ImageCount() const;
	int LayersCount() const;
	const Layer& LayerAt(int index) const;
	vector<unique_ptr<Matrix2D>> ComputeDOGs() const;
	int ImageWidth() const;
	int ImageHeight() const;
private:
	std::vector<unique_ptr<Layer>> layers;	
	int layersCount = 0;
	int imageHeight;
	int imageWidth;
};

