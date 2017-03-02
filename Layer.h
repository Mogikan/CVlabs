#pragma once
#include <vector>
#include <Image.h>
using namespace std;
class Layer
{
public:
	Layer(unique_ptr<Matrix2D> image, double scale, int depth);
	Layer(const Layer& anotherLayer);
	const Matrix2D& ImageD() const;
	const Image& Image() const;
	double Sigma() const;
	double EffectiveSigma() const;
private:
	unique_ptr<Matrix2D> imageD;
	unique_ptr<Image> image;
	double sigma;
	int depth;
};

