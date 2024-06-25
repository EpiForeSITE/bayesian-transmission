#include "util.h"
#include <unsupported/Eigen/MatrixFunctions>

/* Matrix expontial function from Eigen */
/* The only reason that this library is not just header files. */

void util::Markov::expQt(int n, double **Q, double t, double **etQ)
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

/* Default Object writer */

ostream& util::operator<<(ostream &os, Object *x)
{
        if (x == 0)
        {
                os << "null";
        }
        else
        {
                x->write(os);
        }

        return os;
}

/* Class constants */

	long Object::indexcounter = 0;

	double Random::log2pi = log(2*M_PI);
	
	int Vector::defcap = 10;

	int Map::defcap = 10;

	int IntMap::defcap = 10;


/* Utility mathematical functions */

	double util::lbeta(double a, double b)
	{
		return lgamma(a) + lgamma(b) - lgamma(a+b);
	}

	double util::digamma(double x)
	{
		if (x < 6)
			return digamma(x+1) - 1/x;

		double d = log(x) - 0.5 / x;
		double x2 = 1.0/x/x;

		d += (((((((-0.08333333*x2) + 0.0210928)*x2 - 0.007575758)*x2 + 0.00416666667)*x2 -0.002968254)*x2 +0.0083333333)*x2 -0.08333333)*x2;

		return d;
	}

	double util::trigamma(double x)
	{
		if (x < 6)
			return trigamma(x+1) + 1/x/x;

		double d = 1/x + 0.5 *1/x/x;
		double x2 = 1.0/x/x;

		d += (((((((7.0/6.0*x2) - 691.0/2730.0)*x2 + 5.0/66.0)*x2 - 1/30.0)*x2 + 1.0/42.0)*x2 -1/30.0)*x2 + 1.0/6.0)*x2/x;
	
		return d;
	}

	double util::logit(double x)
	{
		return log(x/(1-x));
	}

	double util::logistic(double x)
	{
		return exp(x)/(1+exp(x));
	}

