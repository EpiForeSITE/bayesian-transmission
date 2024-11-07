#ifndef ALUN_MODELING_RANDOMTESTPARAMS_H
#define ALUN_MODELING_RANDOMTESTPARAMS_H

#include "TestParams.h"

class RandomTestParams : public TestParams
{
protected:

	int n;
	double *rates;
	double *shapepar;
	double *ratepar;
	double *rateprior;
	double *shapeprior;
	int *updaterate;

public:

	RandomTestParams(int nst) : TestParams(nst)
	{
		n = 3;
		rates = new double[n];
		shapepar = new double[n];
		ratepar = new double[n];
		shapeprior = new double[n];
		rateprior = new double[n];
		updaterate = new int[n];

		setRates(1,1,1);
		setRatePriors(1,1,1,1,1,1);
		if (nstates == 2)
			setUpdateRates(1,0,1);
		if (nstates == 3)
			setUpdateRates(1,1,1);

		initCounts();
	}

	~RandomTestParams()
	{
		delete [] updaterate;
		delete [] rates;
		delete [] shapepar;
		delete [] ratepar;
		delete [] shapeprior;
		delete [] rateprior;
	}

// Implement Parameters.

	inline double logProb(HistoryLink *h)
	{
		double x = TestParams::logProb(h);
		x += log(rates[stateIndex(h->getPState()->infectionStatus())]);
		return x;
	}

	inline virtual double logProbGap(HistoryLink *g, HistoryLink *h)
	{
		double time = h->getEvent()->getTime()-g->getEvent()->getTime();
		LocationState *s = h->uPrev()->getUState();
		double x = 0;
		x += -time * s->getSusceptible() * rates[0];
		x += -time * s->getLatent() * rates[1];
		x += -time * s->getColonized() * rates[2];
		return x;
	}

	inline void initCounts()
	{
		TestParams::initCounts();
		for (int i=0; i<n; i++)
		{
			ratepar[i] = rateprior[i];
			shapepar[i] = shapeprior[i];
		}
	}

	inline void count(HistoryLink *h)
	{
		TestParams::count(h);
		shapepar[stateIndex(h->getPState()->infectionStatus())] += 1;
	}

	inline virtual void countGap(HistoryLink *g, HistoryLink *h)
	{
		double time = h->getEvent()->getTime() - g->getEvent()->getTime();
		LocationState *s = h->uPrev()->getUState();
		ratepar[0] += time * s->getSusceptible();
		ratepar[1] += time * s->getLatent();
		ratepar[2] += time * s->getColonized();
	}

	virtual inline void update(Random *r, int max)
	{
		TestParams::update(r,max);

		if (max)
		{
			for (int i=0; i<n; i++)
				if (updaterate[i])
					rates[i] = (shapepar[i]-1)/ratepar[i];
		}
		else
		{
			for (int i=0; i<n; i++)
				if (updaterate[i])
					rates[i] = r->rgamma(shapepar[i],ratepar[i]);
		}
	}

	virtual inline void setUpdateRates(int a, int b, int c)
	{
		updaterate[0] = a;
		updaterate[1] = b;
		updaterate[2] = c;
	}


// Personal accessors.

	void setRates(double a, double b, double c)
	{
		rates[0] = a;
		rates[1] = b;
		rates[2] = c;
	}

	void setRatePriors(double va, double pna, double vb, double pnb, double vc, double pnc)
	{
		// Value, #obs pairs.

		double na = pna > 1 ? pna : 1;
		double nb = pnb > 1 ? pnb : 1;
		double nc = pnc > 1 ? pnc : 1;

		shapeprior[0] = va*na;
		rateprior[0] = na;
		shapeprior[1] = vb*nb;
		rateprior[1] = nb;
		shapeprior[2] = vc*nc;
		rateprior[2] = nc;
	}

	virtual int nParam()
	{
		return 2*nstates;
	}

	virtual string *paramNames()
	{
		string *res = new string[2*nstates];

		if (nstates == 3)
		{
			res[0] = "RTest.P(+|unc)";
			res[1] = "RTest.P(+|lat)";
			res[2] = "RTest.P(+|col)";
			res[3] = "RTest.rateUnc";
			res[4] = "RTest.rateLat";
			res[5] = "RTest.rateCol";
		}

		if (nstates == 2)
		{
			res[0] = "RTest.P(+|unc)";
			res[1] = "RTest.P(+|col)";
			res[2] = "RTest.rateUnc";
			res[3] = "RTest.rateCol";
		}

		return res;
	}

	virtual void write (ostream &os)
	{
		TestParams::write(os);
		os << "\t";

		char *buffer = new char[100];
		sprintf(buffer,"%12.10f\t",rates[0]);
		os << buffer;
		if (nstates == 3)
		{
			sprintf(buffer,"%12.10f\t",rates[1]);
			os << buffer;
		}
		sprintf(buffer,"%12.10f",rates[2]);
		os << buffer;
		delete [] buffer;
	}
};
#endif // ALUN_MODELING_RANDOMTESTPARAMS_H
