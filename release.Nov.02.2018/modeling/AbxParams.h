
class AbxParams : public Parameters
{
private:

	int n;
	int nstates;
	double *rates;
	double *ratepar;
	double *shapepar;
	double *priorrate;
	double *priorshape;
	int *doit;

public:

	AbxParams(int k) 
	{
		nstates = k;
		n = 3;
		rates = new double[n];
		shapepar = new double[n];
		ratepar = new double[n];
		priorshape = new double[n];
		priorrate = new double[n];
		doit = new int[n];

		switch(nstates)
		{
		case 2:
			set(1,0,1);
			setUpdate(1,0,1);
			break;
		case 3:
			set(1,1,1);
			setUpdate(1,1,1);
			break;
		}

		setPriors(1,1,1,1,1,1);

		initCounts();
	}

	~AbxParams()
	{
		if (rates)
			delete [] rates;
		if (shapepar)
			delete [] shapepar;
		if (ratepar)
			delete [] ratepar;
		if (priorshape)
			delete [] priorshape;
		if (priorrate)
			delete [] priorrate;
		if (doit)
			delete [] doit;
	}

// Implement Parameters.

	virtual inline double logProb(HistoryLink *h)
	{
		if (h->getEvent()->getType() == abxon)
		{
			AbxPatientState *ps = (AbxPatientState *) h->getPState();
			if (ps->onAbx() == 1)
			{
				return  log (rates[stateIndex(ps->infectionStatus())]);
			}
		}
		return 0;
	}

	virtual inline double logProbGap(HistoryLink *g, HistoryLink *h)
	{
		AbxLocationState *s = (AbxLocationState *) h->uPrev()->getUState();
		return - (h->getEvent()->getTime() - g->getEvent()->getTime()) *
			(
				s->getNoAbxSusceptible() * rates[0] + 
				s->getNoAbxLatent() * rates[1] +
				s->getNoAbxColonized() * rates[2]
			);
	}

	virtual inline void count(HistoryLink *h)
	{
		if (h->getEvent()->getType() == abxon)
		{
			AbxPatientState *ps = (AbxPatientState *) h->getPState();
			if (ps->onAbx() == 1)
				shapepar[stateIndex(ps->infectionStatus())] += 1;
		}
	}

	virtual inline void countGap(HistoryLink *g, HistoryLink *h)
	{
		double time = h->getEvent()->getTime() - g->getEvent()->getTime();
		AbxLocationState *s = (AbxLocationState *) h->uPrev()->getUState();
		ratepar[0] += time * s->getNoAbxSusceptible();
		ratepar[1] += time * s->getNoAbxLatent();
		ratepar[2] += time * s->getNoAbxColonized();
	}

	virtual inline void initCounts()
	{
		for (int i=0; i<n; i++)
		{
			shapepar[i] = priorshape[i];
			ratepar[i] = priorrate[i];
		}
	}

	virtual inline void update(Random *r, int max)
	{
		double *newrates = new double[n];

		if (max)
		{
			for (int i=0; i<n; i++)
				newrates[i] = (doit[i] ? (shapepar[i]-1)/ratepar[i] : rates[i]);
		}
		else
		{
			for (int i=0; i<n; i++)
				newrates[i] = (doit[i] ? r->rgamma(shapepar[i],ratepar[i]) : rates[i]);
		}

		set(newrates[0],newrates[1],newrates[2]);
		delete [] newrates;
	}

// Personal accessors.

	virtual inline void set(double c, double p, double d)
	{
		rates[0] = c;
		rates[1] = p;
		rates[2] = d;
	}

	virtual inline void setPriors(double va, double pna, double vb, double pnb, double vc, double pnc)
	{
		// Value and number of observations pairs.
		double na = pna > 1 ? pna : 1;
		double nb = pnb > 1 ? pnb : 1;
		double nc = pnc > 1 ? pnc : 1;
		
		priorshape[0] = va*na;
		priorrate[0] = na;
		priorshape[1] = vb*nb;
		priorrate[1] = nb;
		priorshape[2] = vc*nc;
		priorrate[2] = nc;
	}

	virtual inline void setUpdate(int c, int p, int d)
	{
		doit[0] = c;
		doit[1] = p;
		doit[2] = d;
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
			res[0] = "Abx.rateUnc";
			res[1] = "Abx.rateLat";
			res[2] = "Abx.rateCol";
		}
	
		if (nstates == 2)
		{
			res[0] = "Abx.rateUnc";
			res[1] = "Abx.rateCol";
		}

		return res;	
	}

	virtual void write (ostream &os)
	{
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
		delete[] buffer;
	}
};
