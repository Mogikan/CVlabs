#include "Matrix2D.h"



Matrix2D::Matrix2D(double values[], int width, int height)
{
	this->width = width;
	this->height = height;
	this->elements = std::vector<double>(values,values+width*height);
}

Matrix2D::Matrix2D(vector<double> imagePixels, int width, int height)
{
	this->elements = imagePixels;
	this->width = width;
	this->height = height;
}

Matrix2D::Matrix2D(int width, int height)
{
	this->elements = std::vector<double>(width*height);
	this->width = width;
	this->height = height;
}

Matrix2D::Matrix2D(const Matrix2D & matrix)
{
	this->elements = std::vector<double>(matrix.elements);
	this->width = matrix.width;
	this->height = matrix.height;
}


double Matrix2D::GetElementAt(int x, int y)
{
	return elements[y*width+x];
}

double Matrix2D::GetElementAt(int plainIndex)
{
	return elements[plainIndex];
}

void Matrix2D::SetElementAt(int x, int y, double value)
{
	elements[y*width + x] = value;
}

void Matrix2D::SetElementAt(int plainIndex, double value)
{
	elements[plainIndex] = value;
}

std::vector<double> Matrix2D::ExtractData()
{

	return elements;
}

int Matrix2D::GetWidth()
{
	return width;
}

int Matrix2D::GetHeight()
{
	return height;
}

int Matrix2D::GetTotalElements()
{
	return GetWidth()*GetHeight();
}

Matrix2D::~Matrix2D()
{
}
