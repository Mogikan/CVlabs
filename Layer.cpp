#include "Layer.h"


Layer::Layer(unique_ptr<Matrix2D> inputImage, double scale)
{
	this->image = move(inputImage);
	this->scale = scale;
}

Layer::Layer(const Layer & anotherLayer)
{
	image = move(make_unique<Matrix2D>(Matrix2D(*anotherLayer.image)));
	scale = anotherLayer.scale;
}

Matrix2D & Layer::GetImage() const
{
	return *image;
}
