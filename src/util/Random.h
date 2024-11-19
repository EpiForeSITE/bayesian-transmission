// util/Random.h
#ifndef ALUN_UTIL_RANDOM_H
#define ALUN_UTIL_RANDOM_H

#include "Object.h"
#include <stdlib.h>

class Random : public Object
{
private:
	double ahrensDieter(double a);
	double chengFeast(double a);

public:
	Random(int seed)
	{
		std::srand(seed);
	}

	Random()
	{
		std::srand(1);
	}

	static double log2pi;

	void setSeed(int seed);

/**
	Probability density functions.
*/

	double logdgamma(double x, double a, double b);
	double logdbeta(double x, double a, double b);
	double logddirichlet(int n, double *p, double *x);
	double logdnorm(double x, double m, double s);
	double logdmillerone(double x, double p, double q, double r, double s);
	double logdexp(double x, double lambda);
/**
	Random number generators.
*/
	double runif();
	double runif(double a, double b);
	double rexp();
	double rexp(double l);
	double rgamma(double a, double b);
	double mcmillerone(int n, double a, double sigma, double p, double q, double r, double s);
	void rmiller(double *ab, double p, double q, double r, double s);
	double rbeta(double a, double b);
	void rdirichlet(int n, double *p, double *x);
	double rchisq(double n);
	double rnorm();
	double rnorm(double m, double s);
	double rscaledinvchisq(double nu, double tau2);
	void rnorminvchisq(double *m, double *s2, double mu, double kappa, double nu, double tau2);
	double rpoisson(double l);
	double logdint(int x, int n, double *pi);
	int rint(int n, double *pi);

};
#endif // ALUN_UTIL_RANDOM_H
