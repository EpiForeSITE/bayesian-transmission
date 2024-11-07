#ifndef ALUN_MODELING_TESTPARAMS_H
#define ALUN_MODELING_TESTPARAMS_H

#include "Parameters.h"

class TestParams : public Parameters
{
protected:

	int nstates;
	int n;
	int m;

	// nxm matrix with probs[i][j] = P(test result == j | colonization status = i)
	double **probs;
	double **logprobs;
	double **counts;
	double **priors;
	int *doit;

public:

	TestParams(int nst)
	{
		nstates = nst;
		n = 3;
		m = 2;

		probs = cleanAlloc(n,m);
		logprobs = cleanAlloc(n,m);
		counts = cleanAlloc(n,m);
		priors = cleanAlloc(n,m);
		doit = cleanAllocInt(n);

		set(0,0,0.80);
		setUpdate(0,0,1);
		setPriors(1,1,1,1,1,1);
		initCounts();
	}

	~TestParams()
	{
		cleanFree(&probs,n);
		cleanFree(&logprobs,n);
		cleanFree(&counts,n);
		cleanFree(&priors,n);
		cleanFree(&doit);
	}

	virtual inline double eventProb(InfectionStatus s, int onabx, EventCode e)
	{
		int i = stateIndex(s);
		int j = testResultIndex(e);
		return ( i < 0 || j < 0 ? 0 : probs[i][j] );
	}

	virtual double *resultProbs(int onabx, EventCode e)
	{
		double *P = cleanAlloc(nstates);

		if (nstates == 2)
		{
			P[0] = eventProb(uncolonized,0,e);
			P[1] = eventProb(colonized,0,e);
		}

		if (nstates == 3)
		{
			P[0] = eventProb(uncolonized,0,e);
			P[1] = eventProb(latent,0,e);
			P[2] = eventProb(colonized,0,e);
		}

		return P;
	}

// Implement Parameters.

	inline virtual double logProb(HistoryLink *h)
	{
		int i = stateIndex(h->getPState()->infectionStatus());
		int j = testResultIndex(h->getEvent()->getType());
		return ( i < 0 || j < 0 ? 0 : logprobs[i][j] );
	}

	inline virtual void initCounts()
	{
		for (int i=0; i<n; i++)
			for (int j=0; j<m; j++)
				counts[i][j] = priors[i][j];
	}

	inline virtual void count(HistoryLink *h)
	{
		int i = stateIndex(h->getPState()->infectionStatus());
		int j = testResultIndex(h->getEvent()->getType());
		if (i < 0 || j < 0)
			return;

		counts[i][j] += 1;
	}

	inline virtual void update(Random *r, int max)
	{
		double *newpos = new double[n];

		if (max)
		{
			for (int i=0; i<n; i++)
				newpos[i] = ( doit[i] ? (counts[i][1]-1)/(counts[i][0]+counts[i][1]-2) : probs[i][1] );
		}
		else
		{
			for (int i=0; i<n; i++)
				newpos[i] = ( doit[i] ? r->rbeta(counts[i][1],counts[i][0]) : probs[i][1] );
		}

		set(newpos[0],newpos[1],newpos[2]);
		delete [] newpos;
	}

// Personal accessors.

	virtual inline void set(int onabx, double punc, double plat, double pcol)
	{
		set(punc,plat,pcol);
	}

	virtual inline void set(double punc, double plat, double pcol)
	{
		probs[0][0] = 1-punc;
		probs[0][1] = punc;
		probs[1][0] = 1-plat;
		probs[1][1] = plat;
		probs[2][0] = 1-pcol;
		probs[2][1] = pcol;

		for (int i=0; i<n; i++)
			for (int j=0; j<m; j++)
				logprobs[i][j] = log(probs[i][j]);
	}

	virtual inline void setPriors(int onabx, double a, double b, double c, double d, double e, double f)
	{
		setPriors(a,b,c,d,e,f);
	}

	virtual inline void setPriors(double a, double b, double c, double d, double e, double f)
	{
		// Probs determined by relative values of pairs. Equivalent #obs found by summing.
		priors[0][0] = a;
		priors[0][1] = b;
		priors[1][0] = c;
		priors[1][1] = d;
		priors[2][0] = e;
		priors[2][1] = f;
	}

	virtual inline void setUpdate(int onabx, int unc, int lat, int col)
	{
		setUpdate(unc,lat,col);
	}

	virtual inline void setUpdate(int unc, int lat, int col)
	{
		doit[0] = unc;
		doit[1] = lat;
		doit[2] = col;
	}

	virtual inline int nParam()
	{
		return nstates;
	}

	virtual string *paramNames()
	{
		string *res = new string[nstates];

		if (nstates == 3)
		{
			res[0] = "Test.P(+|unc)";
			res[1] = "Test.P(+|lat)";
			res[2] = "Test.P(+|col)";
		}

		if (nstates == 2)
		{
			res[0] = "Test.P(+|unc)";
			res[1] = "Test.P(+|col)";
		}

		return res;
	}

	virtual void write (ostream &os)
	{
		char *buffer = new char[100];
		sprintf(buffer,"%12.10f\t",probs[0][1]);
		os << buffer;
		if (nstates == 3)
		{
			sprintf(buffer,"%12.10f\t",probs[1][1]);
			os << buffer;
		}
		sprintf(buffer,"%12.10f",probs[2][1]);
		os << buffer;
		delete [] buffer;
	}
};
#endif // ALUN_MODELING_TESTPARAMS_H
