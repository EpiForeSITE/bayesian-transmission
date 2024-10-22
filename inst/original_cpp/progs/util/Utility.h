

	double lbeta(double a, double b)
	{
		return lgamma(a) + lgamma(b) - lgamma(a+b);
	}

	double digamma(double x)
	{
		if (x < 6)
			return digamma(x+1) - 1/x;

		double d = log(x) - 0.5 / x;
		double x2 = 1.0/x/x;

		d += (((((((-0.08333333*x2) + 0.0210928)*x2 - 0.007575758)*x2 + 0.00416666667)*x2 -0.002968254)*x2 +0.0083333333)*x2 -0.08333333)*x2;

		return d;
	}

	double logit(double x)
	{
		return log(x/(1-x));
	}

	double invlogit(double x)
	{
		return exp(x)/(1+exp(x));
	}

	double dlogit(double x)
	{
		return 1/x/(1-x);
	}
