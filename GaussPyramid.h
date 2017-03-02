#pragma once
#include <vector>
#include "Matrix2D.h"
#include "Octave.h"



using namespace std;
class GaussPyramid
{
public:
	GaussPyramid(
		Matrix2D& originalImage,
		int octaveCount,
		int layersInOctave,
		double sigma0, 
		double sigmaA);
	int OctavesCount();
	const Octave& OctaveAt(int index);	
	void AddOctave(unique_ptr<Octave> octave);
private:
	int octavesCount =0;
	std::vector<unique_ptr<Octave>> octaves;
	double sigma0, sigmaA;
	void BuildOctaves(unique_ptr<Matrix2D> firstImage,int octaveCount, int layersInOctave);
};

