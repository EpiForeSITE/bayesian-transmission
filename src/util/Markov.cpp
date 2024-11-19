#include <unsupported/Eigen/MatrixFunctions>

namespace util{
#include "Markov.h"
/* Matrix exponential function from Eigen */

void Markov::expQt(int n, double **Q, double t, double **etQ)
{
	Eigen::MatrixXd M(n,n);
	for (int i=0; i<n; i++)
		for (int j=0; j<n; j++)
			M(i,j) = Q[i][j];

	M *= t;

	M = M.exp();

	for (int i=0; i<n; i++)
		for (int j=0; j<n; j++)
			etQ[i][j] = M(i,j);
}


} // namespace util
