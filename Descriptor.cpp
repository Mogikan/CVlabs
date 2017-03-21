#include "Descriptor.h"


Descriptor::Descriptor(Point location, vector<double> value):location(location), descriptorValue(value)
{
	Normalize();
	RemoveNoise();
	Normalize();
}

void Descriptor::Normalize()
{
	auto minmaxElement = minmax_element(descriptorValue.begin(), descriptorValue.end());
	auto minElement = minmaxElement.first[0];
	auto maxElement = minmaxElement.second[0];
	transform(descriptorValue.begin(), descriptorValue.end(), descriptorValue.begin(),
		[minElement, maxElement](double element)->double
	{
		return ((element - minElement) / (maxElement - minElement));
	});
}

int Descriptor::Size()
{
	return descriptorValue.size();
}

double Descriptor::operator[](int index)
{
	return descriptorValue.at(index);
}

Point Descriptor::GetPoint()
{
	return location;
}

void Descriptor::RemoveNoise()
{	
	transform(descriptorValue.begin(), descriptorValue.end(), descriptorValue.begin(),
		[&](double element)->double
	{
		return element>Threshold?element:0;
	});
}


