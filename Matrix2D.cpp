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


double Matrix2D::At(int x, int y) const
{
	return elements[y*width+x];
}

double Matrix2D::At(int plainIndex) const
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

std::vector<double> Matrix2D::ExtractData() const
{

	return elements;
}

int Matrix2D::Width() const
{
	return width;
}

int Matrix2D::Height() const
{
	return height;
}

int Matrix2D::TotalElements()
{
	return Width()*Height();
}

double Matrix2D::GetIntensity(int x, int y, BorderMode borderHandlingMode) const
{
	int effectiveX = x;
	int effectiveY = y;
	if (x < 0)
	{
		switch (borderHandlingMode)
		{
		case BorderMode::zero:
			return 0;
			break;
		case BorderMode::extend:
			effectiveX = 0;
			break;
		case BorderMode::mirror:
			effectiveX = -x - 1;
			break;
		case BorderMode::wrap:
			effectiveX = width + x;
			break;
		}
	}
	if (y < 0)
	{
		switch (borderHandlingMode)
		{
		case BorderMode::zero:
			return 0;
			break;
		case BorderMode::extend:
			effectiveY = 0;
			break;
		case BorderMode::mirror:
			effectiveY = -y - 1;
			break;
		case BorderMode::wrap:
			effectiveY = height + y;
			break;
		}
	}
	if (x >= width)
	{
		switch (borderHandlingMode)
		{
		case BorderMode::zero:
			return 0;
			break;
		case BorderMode::extend:
			effectiveX = width - 1;
			break;
		case BorderMode::mirror:
			effectiveX = width - 1 - (x - width);
			break;
		case BorderMode::wrap:
			effectiveX = x - width;
			break;
		}
	}
	if (y >= height)
	{
		switch (borderHandlingMode)
		{
		case BorderMode::zero:
			return 0;
			break;
		case BorderMode::extend:
			effectiveY = height - 1;
			break;
		case BorderMode::mirror:
			effectiveY = height - 1 - (x - height);
			break;
		case BorderMode::wrap:
			effectiveY = x - height;
			break;
		}
	}
	return At(effectiveX, effectiveY);
}

Matrix2D::~Matrix2D()
{
}
