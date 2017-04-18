#pragma once
#include <vector>
#include "Matrix2D.h"
#include "Octave.h"
#include "BlobInfo.h"


using namespace std;
class GaussPyramid
{
public:
	GaussPyramid(
		const Matrix2D& originalImage,
		int octaveCount,
		int layersInOctave=3,
		double sigma0= 1.6,
		double sigmaA=0.5);	

	int OctavesCount();
	const Octave& OctaveAt(int index) const;	
	const Matrix2D& ImageAt(int octave, int layer) const;
	const Layer& LayerAt(int octave, int layer) const;
	double L(int x, int y, double sigma);
	vector<BlobInfo> FindBlobs(double harrisThreshold = 0.035,int windowSize=16);
	~GaussPyramid();
private:
	int octavesCount =0;
	std::vector<unique_ptr<Octave>> octaves;
	double sigma0, sigmaA;
	void AddOctave(unique_ptr<Octave> octave);
	void BuildOctaves(unique_ptr<Matrix2D> firstImage,int octaveCount, int layersInOctave);	
};

