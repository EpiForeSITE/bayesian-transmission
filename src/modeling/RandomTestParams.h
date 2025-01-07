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

		set(1,0,1,1,1,1);
		if (nstates == 2)
			set(1,1,1,0,1,1);
		if (nstates == 3)
			set(1,1,1,1,1,1);
		set(1,2,1,1,1,1);

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

	virtual string header()
	{
		stringstream s;

		if (nstates == 3)
		{
			s << "RTest.Punc";
                	s << "\t" << "RTest.Plat";
                        s << "\t" << "RTest.Pcol";
                        s << "\t" << "RTest.rateUnc";
                        s << "\t" << "RTest.rateLat";
                        s << "\t" << "RTest.rateCol";
		}

		if (nstates == 2)
		{
			s << "RTest.Punc";
                        s << "\t" << "RTest.Pcol";
                        s << "\t" << "RTest.rateUnc";
                        s << "\t" << "RTest.rateCol";
		}

		return s.str();
	}

// Implement Parameters.

	inline double logProb(infect::HistoryLink *h)
	{
		double x = TestParams::logProb(h);
		x += log(rates[stateIndex(h->getPState()->infectionStatus())]);
		return x;
	}

	inline virtual double logProbGap(infect::HistoryLink *g, infect::HistoryLink *h)
	{
		double time = h->getEvent()->getTime()-g->getEvent()->getTime();
	    infect::LocationState *s = h->uPrev()->getUState();
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

	inline void count(infect::HistoryLink *h)
	{
		TestParams::count(h);
		shapepar[stateIndex(h->getPState()->infectionStatus())] += 1;
	}

	inline virtual void countGap(infect::HistoryLink *g, infect::HistoryLink *h)
	{
		double time = h->getEvent()->getTime() - g->getEvent()->getTime();
	    infect::LocationState *s = h->uPrev()->getUState();
		ratepar[0] += time * s->getSusceptible();
		ratepar[1] += time * s->getLatent();
		ratepar[2] += time * s->getColonized();
	}

	virtual inline void update(Random *r, bool max)
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

// Personal accessors.

	virtual inline void set(int i, double value, int update, double prival, double prin)
	{
		TestParams::set(i,value,update,prival,prin);
	}

	virtual inline void set(int israte, int i, double value, int update, double prival, double prin)
	{
		if (!israte)
		{
			set(i,value,update,prival,prin);
			return;
		}

		if (value < 0)
		{
			cerr << "Can't set rate parameter negative.\t" << value << "\n";
			exit(1);
		}
		if (prival < 0)
		{
			cerr << "Can't set rate prior value negative.\t" << prival << "\n";
			exit(1);
		}
		if (prin < 0)
		{
			cerr << "Can't set rate prior observations count negative.\t" << prin << "\n";
			exit(1);
		}

		rates[i] = value;

		updaterate[i] = (update > 0);

		double n = prin > 1 ? prin : 1;
		shapeprior[i] = prival * n;
		rateprior[i] = n;
	}

	virtual int nParam() const
	{
		return 2*nstates;
	}

	virtual std::vector<std::string> paramNames() const
	{
	    std::vector<std::string> res(2*nstates);

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
