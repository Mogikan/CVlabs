#include "Descriptor.h"


Descriptor::Descriptor(Point location, vector<double> values):location(location)
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

Point Descriptor::GetPoint()
{
	return location;
}

void Descriptor::RemoveNoise()
{	
	transform(this->begin(), this->end(), this->begin(),
		[&](double element)->double
	{
		return element>Threshold?element:Threshold;
	});
}


