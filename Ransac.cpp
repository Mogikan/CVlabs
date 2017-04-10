#include "Ransac.h"
#include "DebugHelper.h"
#include <gsl\gsl_linalg.h>
#include <gsl\gsl_blas.h>

Ransac::Ransac()
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

Matrix2D Ransac::FindBestHomography(const vector<pair<Descriptor, Descriptor>>& matches)
{
	// Input matrix of type float
	const int homographyMatchesCount = 4;
	gsl_matrix * A = gsl_matrix_alloc(8, 9);
	gsl_matrix * ATA = (gsl_matrix_alloc(9, 9));
	gsl_matrix *U = gsl_matrix_alloc(9, 9);
	gsl_vector *D = gsl_vector_alloc(9);
	gsl_vector *work = gsl_vector_alloc(9);
	
	gsl_matrix *H = gsl_matrix_alloc(3, 3);
	gsl_vector *vector1 = gsl_vector_alloc(3);
	gsl_vector *vector2 = gsl_vector_alloc(3);
	pair<vector<double>, int> bestMatching = {vector<double>(),0};
	vector<double> runningH(9);
	int N = 20;
	do
	{
		unordered_set<int> indexes;
		for (int i = 0; i < homographyMatchesCount; i++)
		{			
			while (true)
			{
				int nextIndex = rand() % matches.size();
				if (!Contains(indexes, nextIndex))
				{
					indexes.insert(nextIndex);

					double x1 = matches[nextIndex].first.GetPoint().x;
					double y1 = matches[nextIndex].first.GetPoint().y;
					double x2 = matches[nextIndex].second.GetPoint().x;
					double y2 = matches[nextIndex].second.GetPoint().y;
					//A << x1 << y1 << 1 << 0 << 0 << 0 << -x2*x1 << -x2*y1 << -x2 << endr
					//	<< 0 << 0 << 0 << x1 << y1 << 1 << -y2*x1 << -y2 * y1 << -y2 << endr;
					gsl_matrix_set(A,i * 2, 0, x1);
					gsl_matrix_set(A,i * 2, 1, y1);
					gsl_matrix_set(A,i * 2, 2, 1);
					gsl_matrix_set(A,i * 2, 3, 0);
					gsl_matrix_set(A,i * 2, 4, 0);
					gsl_matrix_set(A,i * 2, 5, 0);
					gsl_matrix_set(A,i * 2, 6, -x2 * x1);
					gsl_matrix_set(A,i * 2, 7, -x2*y1);
					gsl_matrix_set(A,i * 2, 8, -x2);
					gsl_matrix_set(A,i * 2 + 1, 0, 0);
					gsl_matrix_set(A,i * 2 + 1, 1, 0);
					gsl_matrix_set(A,i * 2 + 1, 2, 0);
					gsl_matrix_set(A,i * 2 + 1, 3, x1);
					gsl_matrix_set(A,i * 2 + 1, 4, y1);
					gsl_matrix_set(A,i * 2 + 1, 5, 1);
					gsl_matrix_set(A,i * 2 + 1, 6, -y2*x1);
					gsl_matrix_set(A,i * 2 + 1, 7, -y2*y1);
					gsl_matrix_set(A,i * 2 + 1, 8, -y2);
					break;
				}
				else
				{
					continue;
				}
			}
		}
		// Output matrices
		gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1., A, A, 0., ATA);
		gsl_linalg_SV_decomp(ATA, U, D, work);
		for (int i = 0; i < 9; i++)
		{
			auto value = gsl_matrix_get(U, i, 8);
			gsl_matrix_set(H, i / 3, i % 3, value);
			runningH[i] = value;
		}		
		
		double threshold = 10;
		int positiveMatches = 0;
		for (int i = 0; i < matches.size(); i++)
		{
			auto point1 = matches[i].first.GetPoint();
			double x1 = point1.x;
			double y1 = point1.y;
			gsl_vector_set(vector1, 0, x1);
			gsl_vector_set(vector1, 1, y1);
			gsl_vector_set(vector1, 2, 1);
			gsl_blas_dgemv(
				CblasNoTrans, 
				CblasNoTrans,
				H, 
				work,
				0.0, 
				vector2);			
			
			double lambda = gsl_vector_get(vector2,2);
			double x2 = gsl_vector_get(vector2, 0) / lambda;
			double y2 = gsl_vector_get(vector2, 1) / lambda;
			if (sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2)) < threshold)
			{
				positiveMatches++;
			}
		}
		if (bestMatching.second < positiveMatches)
		{
			vector<double> result(9);
			for (int i = 0; i <9; i++)
			{
				result[i] = gsl_matrix_get(H,i/3,i%3);
			}
			bestMatching = {result,positiveMatches};
		}
	} while (0<N--);
	gsl_matrix_free(A);
	gsl_matrix_free(ATA);
	gsl_matrix_free(U);
	gsl_vector_free(D);
	gsl_vector_free(work);

	gsl_matrix_free(H);
	gsl_vector_free(vector1);
	gsl_vector_free(vector2);
	
	return Matrix2D(bestMatching.first,3,3);
}


Ransac::~Ransac()
{
}
