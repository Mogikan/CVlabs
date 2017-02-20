#include "Matrix2D.h"



Matrix2D::Matrix2D(unique_ptr<double[]> elements,int width, int height)
{
	this->elements = move(elements);
	this->width = width;
	this->height = height;

}


double Matrix2D::GetElementAt(int x, int y)
{
	return elements[y*width+x];
}

void Matrix2D::SetElementAt(int x, int y, double value)
{
	elements[y*width + x] = value;
}

unique_ptr<double[]> Matrix2D::ExtractData()
{
	return move(elements);
}

int Matrix2D::GetWidth()
{
	return width;
}

int Matrix2D::GetHeight()
{
	return height;
}

Matrix2D::~Matrix2D()
{
}
