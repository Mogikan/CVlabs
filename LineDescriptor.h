#pragma once
struct LineDescriptor
{
	LineDescriptor()
	{
	}
	LineDescriptor(double ro, double fi)
	{
		this->ro = ro;
		this->fi = fi;
	}
	double ro;
	double fi;
};

