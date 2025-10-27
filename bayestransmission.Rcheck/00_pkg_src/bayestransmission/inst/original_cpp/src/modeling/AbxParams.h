
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
			set(0,1,1,1,1);
			set(1,1,0,1,1);
			set(2,1,1,1,1);
			break;
		case 3:
			set(0,1,1,1,1);
			set(1,1,1,1,1);
			set(2,1,1,1,1);
			break;
		}

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

	virtual string header()
	{
		stringstream s;
		s << "Abx.rateUnc";
		if (nstates == 3)
			s << "\t" << "Abx.rateLat";
		s << "\t" << "Abx.rateCol";
		return s.str();
	}

	virtual int getNStates()
	{
		return nstates;
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

		for (int i=0; i<n; i++)
			rates[i] = newrates[i];

		delete [] newrates;
	}

// Personal accessors.

	virtual inline void set(int i, double value, int update, double prival, double prin)
	{
		if (value < 0)
		{
			cerr << "Can't set rate value negative\t." << value << "\n";
			exit(1);
		}
		if (prival < 0)
		{
			cerr << "Can't set rate prior value negative\t." << prival << "\n";
			exit(1);
		}
		if (prin < 0)
		{
			cerr << "Can't set prior observation count negative\t." << prin << "\n";
			exit(1);
		}
	
		rates[i] = value;
		doit[i] = update;
		double n = prin > 1 ? prin : 1;
		priorshape[i] = prival * n;
		priorrate[i] = n;
	}

	virtual void write(ostream &os)
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
