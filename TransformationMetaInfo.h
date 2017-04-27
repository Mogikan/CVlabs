#pragma once
struct TransformationMetaInfo
{
	TransformationMetaInfo()
	{
	}
	TransformationMetaInfo(double x, double y, double scale, double rotation)
	{
		this->x = x;
		this->y = y;
		this->scale = scale;
		this->rotation = rotation;
	}

	double x;
	double y;
	double scale;
	double rotation;
};


