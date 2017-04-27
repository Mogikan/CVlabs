#include "TransformationHelper.h"
#include "DebugHelper.h"
#include <gsl\gsl_linalg.h>
#include <gsl\gsl_blas.h>
#include "MathHelper.h"
#include <random>
using namespace std;
TransformationHelper::TransformationHelper()
{
}

void InitAffineSystem(
	const vector<pair<Descriptor, Descriptor>>& matches,
	const vector<int>& indexes,
	gsl_matrix& A,
	gsl_vector& b	
)
{
	for (int i = 0; i < indexes.size(); i++)
	{
		double x1 = matches[indexes[i]].first.GetPoint().x;
		double y1 = matches[indexes[i]].first.GetPoint().y;
		double x2 = matches[indexes[i]].second.GetPoint().x;
		double y2 = matches[indexes[i]].second.GetPoint().y;
		gsl_matrix_set(&A, i * 2, 0, x1);
		gsl_matrix_set(&A, i * 2, 1, y1);
		gsl_matrix_set(&A, i * 2, 2, 1.0);
		gsl_matrix_set(&A, i * 2, 3, 0.0);
		gsl_matrix_set(&A, i * 2, 4, 0.0);
		gsl_matrix_set(&A, i * 2, 5, 0.0);

		gsl_matrix_set(&A, i * 2 + 1, 0, 0.0);
		gsl_matrix_set(&A, i * 2 + 1, 1, 0.0);
		gsl_matrix_set(&A, i * 2 + 1, 2, 0.0);
		gsl_matrix_set(&A, i * 2 + 1, 3, x1);
		gsl_matrix_set(&A, i * 2 + 1, 4, y1);
		gsl_matrix_set(&A, i * 2 + 1, 5, 1.0);

		gsl_vector_set(&b, i * 2, x2);
		gsl_vector_set(&b, i * 2 + 1, y2);
	}
}

Matrix2D TransformationHelper::CalculateAffineTransform(const vector<pair<Descriptor, Descriptor>>& matches, const vector<int> inliers)
{//Ax=b
	gsl_matrix* A = gsl_matrix_alloc(inliers.size() * 2, 6);;
	gsl_matrix* ATA = gsl_matrix_alloc(6, 6);
	gsl_matrix* ATAinv = gsl_matrix_alloc(6, 6);
	gsl_vector* b = gsl_vector_alloc(2 * inliers.size());
	InitAffineSystem(matches, inliers, *A, *b);

	gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1., A, A, 0., ATA);
	int s;
	gsl_permutation * p = gsl_permutation_alloc(6);
	gsl_linalg_LU_decomp(ATA, p, &s);
	gsl_linalg_LU_invert(ATA, p, ATAinv);
	//ATb
	
	gsl_vector* ATb = gsl_vector_alloc(6);
	gsl_vector* AffineVector = gsl_vector_alloc(6);

	gsl_blas_dgemv(CblasTrans, 1.0, A, b, 0.0, ATb);
	gsl_blas_dgemv(CblasNoTrans, 1, ATAinv, ATb, 0, AffineVector);

	double transformData[9];
	std::fill(transformData, transformData +9, 0);
	for (int i = 0; i < AffineVector->size; i++)
	{
		transformData[i] = gsl_vector_get(AffineVector,i);
	}
	transformData[8] = 1;
	gsl_matrix_free(A);
	gsl_matrix_free(ATA);
	gsl_matrix_free(ATAinv);
	gsl_vector_free(b);
	return Matrix2D(transformData,3,3);
}


bool Contains(unordered_set<int> values, int index)
{
	auto got = values.find(index);
	if (got == values.end())
	{
		return false;
	}
	return true;
}



void InitHomographyMatrix(
	const vector<pair<Descriptor, Descriptor>>& matches,
	gsl_matrix& A,
	const vector<int>& indexes
)
{
	for (int i = 0; i < indexes.size(); i++)
	{
		double x1 = matches[indexes[i]].second.GetPoint().x;
		double y1 = matches[indexes[i]].second.GetPoint().y;
		double x2 = matches[indexes[i]].first.GetPoint().x;
		double y2 = matches[indexes[i]].first.GetPoint().y;

		gsl_matrix_set(&A, i * 2, 0, x1);
		gsl_matrix_set(&A, i * 2, 1, y1);
		gsl_matrix_set(&A, i * 2, 2, 1);
		gsl_matrix_set(&A, i * 2, 3, 0);
		gsl_matrix_set(&A, i * 2, 4, 0);
		gsl_matrix_set(&A, i * 2, 5, 0);
		gsl_matrix_set(&A, i * 2, 6, -x2 * x1);
		gsl_matrix_set(&A, i * 2, 7, -x2*y1);
		gsl_matrix_set(&A, i * 2, 8, -x2);
		gsl_matrix_set(&A, i * 2 + 1, 0, 0);
		gsl_matrix_set(&A, i * 2 + 1, 1, 0);
		gsl_matrix_set(&A, i * 2 + 1, 2, 0);
		gsl_matrix_set(&A, i * 2 + 1, 3, x1);
		gsl_matrix_set(&A, i * 2 + 1, 4, y1);
		gsl_matrix_set(&A, i * 2 + 1, 5, 1);
		gsl_matrix_set(&A, i * 2 + 1, 6, -y2*x1);
		gsl_matrix_set(&A, i * 2 + 1, 7, -y2*y1);
		gsl_matrix_set(&A, i * 2 + 1, 8, -y2);

	}
}

vector<int> GetInliers(const vector<pair<Descriptor, Descriptor>>& matches,
	gsl_vector& vector1,
	gsl_vector& vector2,
	double threshold,
	gsl_matrix& H
	)
{
	vector<int> inliersIndexes;
	
	for (int i = 0; i < matches.size(); i++)
	{
		auto point1 = matches[i].second.GetPoint();
		double x1 = point1.x;
		double y1 = point1.y;
		auto point2 = matches[i].first.GetPoint();
		double x2 = point2.x;
		double y2 = point2.y;
		gsl_vector_set(&vector1, 0, x1);
		gsl_vector_set(&vector1, 1, y1);
		gsl_vector_set(&vector1, 2, 1);
		gsl_blas_dgemv(
			CblasNoTrans,
			1,
			&H,
			&vector1,
			0,
			&vector2);

		double scale = gsl_vector_get(&vector2, 2);
		double transformedX = gsl_vector_get(&vector2, 0) / scale;
		double transformedY = gsl_vector_get(&vector2, 1) / scale;
		if (hypot(transformedX - x2,transformedY - y2) < threshold)
		{
			inliersIndexes.push_back(i);
		}
	}
	return inliersIndexes;
}

vector<double> PrepareTransformVector(const gsl_matrix& H)
{
	vector<double> result(9);
	for (int i = 0; i < 9; i++)
	{
		auto value = gsl_matrix_get(&H, i / 3, i % 3);
		result[i] = value;
	}
	double factor = result[8];
	transform(result.begin(), result.end(), result.begin(),
		[factor]
	(double element)
	{
		return element / factor;
	});
	return result;
}

void CalculateHomograhy(
	const vector<pair<Descriptor, Descriptor>>& matches,
	gsl_matrix& A,
	const vector<int>& indexes,
	gsl_matrix& ATA,
	gsl_matrix& V,
	gsl_vector& S,
	gsl_matrix& H,
	gsl_vector& work
) 
{
	InitHomographyMatrix(matches, A, indexes);
	gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1., &A, &A, 0., &ATA);
	gsl_linalg_SV_decomp(&ATA, &V, &S, &work);
	for (int i = 0; i < 9; i++)
	{
		auto value = gsl_matrix_get(&V, i, 8);
		gsl_matrix_set(&H, i / 3, i % 3, value);
	}
}

Matrix2D TransformationHelper::FindBestHomography(
	const vector<pair<Descriptor, Descriptor>>& matches,
	int N,
	double threshold
)
{
	if (matches.size() < 4)
	{
		throw "Bad Match";
	}

	gsl_matrix * A = gsl_matrix_alloc(8, 9);
	gsl_matrix * ATA = gsl_matrix_alloc(9, 9);
	gsl_matrix *V = gsl_matrix_alloc(9, 9);
	gsl_vector *S = gsl_vector_alloc(9);
	gsl_vector *work = gsl_vector_alloc(9);
	gsl_matrix *H = gsl_matrix_alloc(3, 3);
	gsl_vector *vector1 = gsl_vector_alloc(3);
	gsl_vector *vector2 = gsl_vector_alloc(3);
	vector<int> inliers;
	vector<int> bestMatch;
	vector<int> indexes;	 
	for (int i = 0; i < matches.size(); i++)
	{
		indexes.push_back(i);
	}
	vector<int> random4Points(4);
	do
	{		
		shuffle(indexes.begin(), indexes.end(), mt19937{});
		random4Points[0] = indexes[0];
		random4Points[1] = indexes[1];
		random4Points[2] = indexes[2];
		random4Points[3] = indexes[3];			
		CalculateHomograhy(matches, *A,random4Points ,*ATA,*V,*S,*H,*work);
		inliers = GetInliers(matches, *vector1, *vector2, threshold,*H);
		
		if (bestMatch.size() < inliers.size())
		{					
			bestMatch = inliers;
		}
	} while (0<N--);
	gsl_matrix_free(A);
	A = gsl_matrix_alloc(bestMatch.size()*2, 9);
	CalculateHomograhy(matches, *A, bestMatch, *ATA, *V, *S, *H, *work);
	auto& bestTransform = PrepareTransformVector(*H);
	gsl_matrix_free(A);
	gsl_matrix_free(ATA);
	gsl_matrix_free(V);
	gsl_vector_free(S);
	gsl_vector_free(work);

	gsl_matrix_free(H);
	gsl_vector_free(vector1);
	gsl_vector_free(vector2);	
	return Matrix2D(bestTransform,3,3);
}

