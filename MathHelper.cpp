#include "MathHelper.h"
using namespace std;


MathHelper::MathHelper()
{
}

double MathHelper::ComputeGaussAxesValue(int x, double sigma)
{
	return 1 / (sqrt(2 * M_PI)*sigma)*exp(-x*x / (2 * sigma*sigma));
}

pair<double, double> MathHelper::Eigenvalues(Matrix2D & matrix)
{
	if (!(matrix.Width() == 2 && matrix.Height() == 2))
	{
		throw new exception("Implemented only for 2x2");
	}
	double a0 = matrix.At(0, 0);
	double b0 = matrix.At(0, 1);
	double c0 = matrix.At(1, 1);
	double a = 1;
	double b = -c0 - a0;
	double c = a0*c0 - b0*b0;
	double determinant = b*b - 4 * a*c;
	double lambda1 = (-b + sqrt(determinant)) / (2 * a);
	double lambda2 = (-b - sqrt(determinant)) / (2 * a);
	return{ lambda1, lambda2 };
}


MathHelper::~MathHelper()
{
}
