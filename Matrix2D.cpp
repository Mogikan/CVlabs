#include "Matrix2D.h"
#include <algorithm>


Matrix2D::Matrix2D(double values[], int width, int height)
{
	this->width = width;
	this->height = height;
	this->elements = std::vector<double>(values,values+width*height);
}

Matrix2D::Matrix2D(const vector<double>& imagePixels, int width, int height)
{
	this->elements.resize(width*height);
	copy(imagePixels.begin(), imagePixels.end(),this->elements.begin());
	this->width = width;
	this->height = height;
}

Matrix2D::Matrix2D(int width, int height)
{
	this->elements.resize(width*height);
	this->width = width;
	this->height = height;
}

Matrix2D::Matrix2D(const Matrix2D & matrix)
{
	this->elements.resize(matrix.elements.size());
	copy(matrix.elements.begin(), matrix.elements.end(), this->elements.begin());
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

unique_ptr<Matrix2D> Matrix2D::Normalize() const
{
	vector<double> result(this->width*this->height,0.0);
	auto minmaxElement = minmax_element(elements.begin(), elements.end());
	auto minElement = minmaxElement.first[0];
	auto maxElement = minmaxElement.second[0];
	transform(elements.begin(), elements.end(), result.begin(),
		[minElement, maxElement](double element)->double
	{
		return ((element - minElement) / (maxElement - minElement));
	});
	return make_unique<Matrix2D>(result,this->width,this->height);
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

unique_ptr<Matrix2D> Matrix2D::operator-(const Matrix2D & secondMatrix) const
{
	vector<double> result(this->width*this->height);
	auto& first = this->ExtractData();
	auto& second = secondMatrix.ExtractData();
	std::transform(
		first.begin(),
		first.end(),
		second.begin(),
		result.begin(),
		std::minus<double>());
	return make_unique<Matrix2D>(result,this->width,this->height);
}
