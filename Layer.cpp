#include "Layer.h"


Layer::Layer(unique_ptr<Matrix2D> inputImage, double sigma,int depth)
{
	this->imageD = move(inputImage);
	this->image = make_unique<Image>(*imageD);
	this->sigma = sigma;
	this->depth = depth;
}

Layer::Layer(const Layer & anotherLayer)
{
	imageD = make_unique<Matrix2D>(*anotherLayer.imageD);	
	sigma = anotherLayer.sigma;
	depth = anotherLayer.depth;
}

const Matrix2D & Layer::GetImage() const
{
	return *imageD;
}

double Layer::Sigma() const
{
	return sigma;
}

double Layer::EffectiveSigma() const
{
	return sigma * pow(2,depth);
}
