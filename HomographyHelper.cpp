#include "HomographyHelper.h"
#include "DebugHelper.h"
#include <gsl\gsl_linalg.h>
#include <gsl\gsl_blas.h>
#include "MathHelper.h"
HomographyHelper::HomographyHelper()
{
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

void InitMatrixA(
	const vector<pair<Descriptor,Descriptor>>& matches,
	gsl_matrix& A
	)
{
	unordered_set<int> indexes;
	const int homographyMatchesCount = 4;
	for (int i = 0; i < homographyMatchesCount; i++)
	{
		while (true)
		{
			int nextIndex = rand() % matches.size();
			if (!Contains(indexes, nextIndex))
			{
				indexes.insert(nextIndex);

				double x1 = matches[nextIndex].second.GetPoint().x;
				double y1 = matches[nextIndex].second.GetPoint().y;
				double x2 = matches[nextIndex].first.GetPoint().x;
				double y2 = matches[nextIndex].first.GetPoint().y;

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
				break;
			}
			else
			{
				continue;
			}
		}
	}
}

int CalculateMatches(const vector<pair<Descriptor, Descriptor>>& matches,
	gsl_vector& vector1,
	gsl_vector& vector2,
	double threshold,
	gsl_matrix& H
	)
{
	int positiveMatches = 0;
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
		if (sqrt(MathHelper::Sqr(transformedX - x2) + MathHelper::Sqr(transformedY - y2)) < threshold)
		{
			positiveMatches++;
		}
	}
	return positiveMatches;
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

Matrix2D HomographyHelper::FindBestHomography(
	const vector<pair<Descriptor, Descriptor>>& matches,
	int N,
	double threshold
)
{
	if (matches.size() < 4)
	{
		throw "Bad Match";
	}
	// Input matrix of type float
	
	gsl_matrix * A = gsl_matrix_alloc(8, 9);	
	gsl_matrix * ATA = gsl_matrix_alloc(9, 9);
	gsl_matrix *V = gsl_matrix_alloc(9, 9);
	gsl_vector *S = gsl_vector_alloc(9);
	gsl_vector *work = gsl_vector_alloc(9);	
	gsl_matrix *H = gsl_matrix_alloc(3, 3);
	gsl_vector *vector1 = gsl_vector_alloc(3);
	gsl_vector *vector2 = gsl_vector_alloc(3);
	int positiveMatches;
	pair<vector<double>, int> bestMatching = {vector<double>(),0};	
	do
	{
		InitMatrixA(matches, *A);
		// Output matrices
		gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1., A, A, 0., ATA);
		gsl_linalg_SV_decomp(ATA, V, S, work);
		for (int i = 0; i < 9; i++)
		{
			auto value = gsl_matrix_get(V, i, 8);
			gsl_matrix_set(H, i / 3, i % 3, value);			
		}		
		positiveMatches = CalculateMatches(matches, *vector1, *vector2, threshold,*H);
		
		if (bestMatching.second < positiveMatches)
		{					
			bestMatching = {PrepareTransformVector(*H),positiveMatches};
		}
	} while (0<N--);
	gsl_matrix_free(A);
	gsl_matrix_free(ATA);
	gsl_matrix_free(V);
	gsl_vector_free(S);
	gsl_vector_free(work);

	gsl_matrix_free(H);
	gsl_vector_free(vector1);
	gsl_vector_free(vector2);	
	return Matrix2D(bestMatching.first,3,3);
}

