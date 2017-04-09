#include "Descriptor.h"


Descriptor::Descriptor(vector<double> values, Point location,double angle,double sigma):location(location),angle(angle),sigma(sigma)
{	
	this->resize(values.size());
	copy(values.begin(), values.end(), this->begin());
	Normalize();
	RemoveNoise();
	Normalize();
}

void Descriptor::Normalize()
{
	auto minmaxElement = minmax_element(this->begin(), this->end());
	auto minElement = minmaxElement.first[0];
	auto maxElement = minmaxElement.second[0];
	transform(this->begin(), this->end(), this->begin(),
		[minElement, maxElement](double element)->double
	{
		return ((element - minElement) / (maxElement - minElement));
	});
}

Point Descriptor::GetPoint() const
{
	return location;
}

double Descriptor::Sigma() const
{
	return sigma;
}

double Descriptor::Angle() const
{
	return angle;
}

void Descriptor::RemoveNoise()
{	
	transform(this->begin(), this->end(), this->begin(),
		[&](double element)->double
	{
		return element>Threshold?element:Threshold;
	});
}


