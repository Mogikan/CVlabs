#include "Ransac.h"
#include "DebugHelper.h"


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
	arma::mat A(homographyMatchesCount*2,9);
	pair<vector<double>, int> bestMatching = {vector<double>(),0};
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
					A(i * 2, 0) = x1;
					A(i * 2, 1) = y1;
					A(i * 2, 2) = 1;
					A(i * 2, 3) = 0;
					A(i * 2, 4) = 0;
					A(i * 2, 5) = 0;
					A(i * 2, 6) = -x2 * x1;
					A(i * 2, 7) = -x2*y1;
					A(i * 2, 8) = -x2;
					A(i * 2 + 1, 0) = 0;
					A(i * 2 + 1, 1) = 0;
					A(i * 2 + 1, 2) = 0;
					A(i * 2 + 1, 3) = x1;
					A(i * 2 + 1, 4) = y1;
					A(i * 2 + 1, 5) = 1;
					A(i * 2 + 1, 6) = -y2*x1;
					A(i * 2 + 1, 7) = -y2*y1;
					A(i * 2 + 1, 8) = -y2;
					break;
				}
				else
				{
					continue;
				}
			}
		}
		for (int i = 0; i < 2*4; i++)
		{
			DebugHelper::WriteToDebugOutput("<<");
			for (int j = 0; j < 9; j++)
			{
				DebugHelper::WriteToDebugOutput(A(i, j));
			}
		}

		// Output matrices
		arma::mat U;
		arma::vec S;
		arma::mat V;

		// Perform SVD
		auto& ata =( A.t()*A).eval();
		arma::svd(U, S, V, ata);
		DebugHelper::WriteToDebugOutput("ATA");
		for (int i = 0; i < ata.n_rows; i++)
		{
			for (int j = 0; j < ata.n_cols; j++)
			{
				DebugHelper::WriteToDebugOutput(ata(i, j));
			}
		}
		DebugHelper::WriteToDebugOutput("V(8)");
			for (int i = 0; i < 9; i++)
			{
				DebugHelper::WriteToDebugOutput(V(i, 8));
			}

			DebugHelper::WriteToDebugOutput("U(8)");
			for (int i = 0; i < 9; i++)
			{
				DebugHelper::WriteToDebugOutput(U(i, 8));
			}
		
		//V-> is our H
		double threshold = 0.5;
		int positiveMatches = 0;
		for (int i = 0; i < matches.size(); i++)
		{
			auto point1 = matches[i].first.GetPoint();
			double x1 = point1.x;
			double y1 = point1.y;
			arma::mat point1Affine(3, 1);
			point1Affine
				<< x1 << arma::endr
				<< y1 << arma::endr
				<< 1 << arma::endr;
			arma::mat h(3, 3);
			for (int i = 0; i < 9; i++)
			{
				h(i / 3, i % 3) = V(i, 8);
			}
			auto& transformedVector = (h*point1Affine).eval();
			double x2 = transformedVector(0, 0);
			double y2 = transformedVector(1, 0);
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
				result[i] = V(i,8);
			}
			bestMatching = {result,positiveMatches};
		}
	} while (0<N--);
	
	
	return Matrix2D(bestMatching.first,3,3);
}


Ransac::~Ransac()
{
}
