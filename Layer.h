#pragma once
#include <vector>
#include <Image.h>
using namespace std;
class Layer
{
public:
	Layer(unique_ptr<Matrix2D> image, double scale);
	Layer(const Layer& anotherLayer);
	Matrix2D& GetImage() const;
private:
	unique_ptr<Matrix2D> image;
	double scale;
};

