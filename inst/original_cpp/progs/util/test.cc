#include <stdio.h>
#include <iostream>

#include "util.h"

int main(int argc, char *argv[])
{
	try
	{
/*
		int nsims = 10000;

		MVNormalRandom *R = new MVNormalRandom();

		int p = 1;
		
		double *mu = new double[p];
		double *mm = new double[p];
		double **ss = new double*[p];
		double **psi = new double*[p];
		for (int i=0; i<p; i++)
		{
			ss[i] = new double[p];
			psi[i] = new double[p];
		}

		double m = 0;
		double s2 = 0;
		int n = 100;

		double mean = 0.1006032;
		double sumsq = 87.1197;

		mu[0] = mean;
		psi[0][0] = sumsq;
	
		for (int i=0; i<nsims; i++)
		{
			R->rnorminvwishart(mm,ss,p,mu,n,n,psi);
			cout << mm[0] << "\t" << ss[0][0] << "\t";
			
			R->rnorminvchisq(&m, &s2, mean, n, n, sumsq/n );
			cout << m << "\t" << s2 << "\n";
		}
*/

		int seed = 1;
		
		switch(argc)
		{
                case 2: sscanf(argv[1],"%d",&seed);
		}

		Random *R = new Random(seed);

		List *l = new List();
		l->append(new Integer(1));
		l->append(new Integer(2));
		l->append(new Integer(3));
		l->append(new Integer(4));

		for (int i=0; i<100; i++)
			cout << l->random(R) << "\n";

/*
		//double x = 1;
		double p = -59.07;
		double q = 104.7;
		double r = 105;
		double s = 105;
		//double sigma = 0.1;

		double *ab = new double[2];
		ab[0] = 1;

		for (int i=0; i<10000; i++)
		{
			R->rmiller(ab,p,q,r,s);
			cout << ab[0] << "\t" << ab[1] << "\n";
		}
*/

/*
		for (int i=0; i<100000; i++)
			cout << R->rpoisson(10) << "\n";
*/
	}
	catch (exception *e)
	{
		cerr << "Exception caught.\n" << e->what() << "\n";
	}
}
