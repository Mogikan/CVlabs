#pragma once
#include <vector>
#include "Matrix2D.h"
#include "Image.h"
using namespace std;
class Layer
{
public:
	Layer(unique_ptr<Matrix2D> image, double scale, int depth);
	Layer(const Layer& anotherLayer);
	const Matrix2D& GetImage() const;
	//const Image& GetImage() const;
	double Sigma() const;
	double EffectiveSigma() const;
private:
	unique_ptr<Matrix2D> imageD;
	unique_ptr<Image> image;
	double sigma;
	int depth;
};

