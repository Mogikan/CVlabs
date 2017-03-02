#include "Layer.h"


Layer::Layer(unique_ptr<Matrix2D> inputImage, double sigma,int depth)
{
	this->image = move(inputImage);
	this->sigma = sigma;
	this->depth = depth;
}

Layer::Layer(const Layer & anotherLayer)
{
	image = move(make_unique<Matrix2D>(*anotherLayer.image));
	sigma = anotherLayer.sigma;
	depth = anotherLayer.depth;
}

const Matrix2D & Layer::Image() const
{
	return *image;
}

double Layer::Sigma() const
{
	return sigma;
}

double Layer::EffectiveSigma() const
{
	return sigma * pow(2,depth);
}
