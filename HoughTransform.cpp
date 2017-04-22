#include "HoughTransform.h"



HoughTransform::HoughTransform()
{
}

void HoughTransform::Apply(const Matrix2D & edges, const Matrix2D & magnitude, const Matrix2D & directions)
{
	for (int y = 0; y < edges.Height(); y++)
	{
		for (int x = 0; x < edges.Width(); x++)
		{
			if (edges.At(x, y) > 0)
			{

			}
		}
	}
}


HoughTransform::~HoughTransform()
{
}
