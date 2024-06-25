

class Random : public Object
{
private:
	double ahrensDieter(double a)
	{
		while (true)
		{
			double u = runif();

			if (u <= M_E / (a+M_E))
			{
				double x = pow( (a+M_E)*u/M_E , 1/a );
				if (runif() <= exp(-x))
					return x;
			}
			else
			{
				double x = -log((a+M_E)*(1-u)/a/M_E);
				if (runif() <= pow(x,a-1))
					return x;
			}
		}	
	}

	double chengFeast(double a)
	{
		double b1 = 2.5;
		double b2 = 1.86;

		double c1 = a-1;
                double c2 = (a-1.0/6.0/a)/c1;
                double c3 = 2/c1;
                double c4 = c3+2;
                double c5 = 1/sqrt(a);

		double u = 0;
		double v = 0;
		double w = 0;

		do
		{
			do
			{
				u = runif();
				v = runif();
				if (a > b1)
					u = v + c5 * (1 - b2*u);
			}
			while(u>1 || u < 0);

			w = c2 * v/u;
		}
		while (c3*u+w+1/w > c4 && c3*log(u)-log(w)+w > 1);

		return c1*w;
	}

/*
	double logpregamma(double a, double b, double p, double q, double r)
	{
		return r * a * log(b) - r * lgamma(a) + (a-1) * p - b * q;
	}
*/

public:
	Random(int seed)
	{
		srandom(seed);
	}

	Random()
	{
		srandom(1);
	}

	static double log2pi;

/**
	Probability density functions.
*/
	double logdgamma(double x, double a, double b)
	{
		return a * log(b) + (a-1) * log(x) - b * x - lgamma(a);
	}
	
	double logdbeta(double x, double a, double b)
	{
		return (a-1) * log(x) + (b-1) * log(1-x) - lgamma(a) - lgamma(b) + lgamma(a+b);
	}
	
	double logdnorm(double x, double m, double s)
	{
		return -0.5*log2pi - log(s) - 0.5 * (x-m)*(x-m)/s/s;
	}

	double logdmillerone(double x, double p, double q, double r, double s)
	// p = sum logs type param, q = sum, r = n, s = n;
	{
		return lgamma(x*s) - r * lgamma(x) + x*p - x*s* log(q);
	}

/**
	Random number generators.
*/
	double runif()
	{
		return random() / (double) RAND_MAX;
	}
	
	double runif(double a, double b)
	{
		return a + runif()*(b-a);
	}

	double rexp()
	{
		return -log(runif());
	}
	
	double rexp(double l)
	{
		return rexp()/l;
	}

	double rgamma(double a, double b)
	{
		return ( a <= 1 ? ahrensDieter(a) : chengFeast(a) ) /b;
	}

	double mcmillerone(int n, double a, double sigma, double p, double q, double r, double s)
	{
		double x = a;
		double y = 0;
		
		for (int i=0; i<n; i++)
		{
			y = exp(log(x) + rnorm(0,sigma));
			if (log(runif()) < logdmillerone(y,p,q,r,s) + log(y) -logdmillerone(x,p,q,r,s) - log(x))
				x = y;
		}

		return x;
	}

	void rmiller(double *ab, double p, double q, double r, double s)
	{
		ab[0] = mcmillerone(100,ab[0],0.1,p,q,r,s);
		ab[1] = rgamma(ab[0]*s,q);
	}

	double rbeta(double a, double b)
	{
		double x = rgamma(a,1);
		double y = rgamma(b,1);
		return x/(x+y);
	}

	double rchisq(double n)
	{
		return rgamma(n/2,0.5);
	}

	double rnorm()
	{
		return sqrt(-2*log(runif())) * sin(2*M_PI*runif());
	}

	double rnorm(double m, double s)
	{
		return m + s * rnorm();
	}

	double rscaledinvchisq(double nu, double tau2)
	{
		return nu * tau2 / rchisq(nu);
	}

	void rnorminvchisq(double *m, double *s2, double mu, double kappa, double nu, double tau2)
	{
		*s2 = rscaledinvchisq(nu,tau2);
		*m = mu + rnorm() * sqrt(*s2/kappa);
	}

	double rpoisson(double l)
	{
		double L = exp(-l);
		double p = 1;
		int k = 0;

		do
		{
			k++;
			p *= runif();
		}
		while (p > L);

		return k - 1;
	}
};

double Random::log2pi = log(2*M_PI);
