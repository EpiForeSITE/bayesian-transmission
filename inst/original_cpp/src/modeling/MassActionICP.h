
class MassActionICP : public InColParams 
{
private:

	int n;
	double *rates;
	double *logrates;
	double *ratepar;
	double *shapepar;
	double *priorrate;
	double *priorshape;
	int *doit;

	// value 0 == frequency dependent; 1 == density dependent; 2 == constant;
	int isDensity;

	// The following function should be the only thing depending on 
	// isDensity in the whole class.
	virtual inline double acquisitionFactor(int c, int n)
	{
		switch(isDensity)
		{
		case 2: return 1;
		case 1: return c;
		case 0: return (n > 0 ? c / (double) n : 0);
		default:
			return 0;
		}
	}

protected:

	virtual inline void set(int i, double value)
	{
		rates[i] = value;
		logrates[i] = log(rates[i]);
	}

public:

	MassActionICP(int k, int isdens) : InColParams(k)
	{
		isDensity = isdens;

		n = 3;
		rates = new double[n];
		logrates = new double[n];
		shapepar = new double[n];
		ratepar = new double[n];
		priorshape = new double[n];
		priorrate = new double[n];
		doit = new int[n];

		switch(nstates)
		{
		case 2:
			set(0,0.001,1,1,1);
			set(1,0,0,1,1);
			set(2,0.01,1,1,1);
			break;
		case 3:
			set(0,0.001,1,1,1);
			set(1,0.01,1,1,1);
			set(2,0.01,1,1,1);
			break;
		}

		initCounts();
	}

	~MassActionICP()
	{
		if (rates)
			delete [] rates;
		if (logrates)
			delete [] logrates;
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
		s <<  "MAICP.acq";
		if (nstates == 3)
			s << "\t" << "MAICP.pro";
		s << "\t" << "MAICP.clr";
		return s.str();
	}

// Implement InColParams.

	virtual double *acquisitionRates(double time, PatientState *p, LocationState *s)
	{
		double *P = new double[nstates];

		if (nstates == 2)
		{
			P[0] = rates[0] * acquisitionFactor(s->getColonized(),s->getTotal());
			P[1] = rates[0] * acquisitionFactor(1+s->getColonized(),s->getTotal());
		}

		if (nstates == 3)
		{
			P[0] = rates[0] * acquisitionFactor(s->getColonized(),s->getTotal());
			P[1] = rates[0] * acquisitionFactor(s->getColonized(),s->getTotal());
			P[2] = rates[0] * acquisitionFactor(1+s->getColonized(),s->getTotal());
		}

		return P;
	}

	virtual inline double eventRate(double time, EventCode c, PatientState *p, LocationState *s)
	{
		switch(c)
		{
		case acquisition:
			return rates[0] * acquisitionFactor(s->getColonized(),s->getTotal());
		case progression:
			return rates[1];
		case clearance:
			return rates[2];
		default:
			return 0;
		}
	}

	virtual double **rateMatrix(double time, PatientState *p, LocationState *u)
	{
		double **Q = cleanAlloc(nstates,nstates);

		if (nstates == 2)
		{
			Q[0][1] = eventRate(time,acquisition,p,u);
			Q[0][0] = -Q[0][1];
			Q[1][0] = eventRate(time,clearance,p,u);
			Q[1][1] = -Q[1][0];
		}

		if (nstates == 3)
		{
			Q[0][1] = eventRate(time,acquisition,p,u);
			Q[0][0] = -Q[0][1];
			Q[1][2] = eventRate(time,progression,p,u);
			Q[1][1] = -Q[1][2];
			Q[2][0] = eventRate(time,clearance,p,u);
			Q[2][2] = -Q[2][0];
		}
	
		return Q;
	}
// Implement Parameters.

	virtual inline double logProb(HistoryLink *h)
	{
		switch(h->getEvent()->getType())
		{
		case acquisition:
		case progression:
		case clearance:
			return log(eventRate(h->getEvent()->getTime(),h->getEvent()->getType(),h->pPrev()->getPState(),h->uPrev()->getUState()));
		default:
			return 0;
		}
	}

/*
	This will fail if eventRate depends on PatientStatus.
*/
	virtual inline double logProbGap(HistoryLink *g, HistoryLink *h)
	{
		LocationState *s = h->uPrev()->getUState();
		return - (h->getEvent()->getTime() - g->getEvent()->getTime()) *
		(
			(s->getSusceptible() == 0 ? 0 : s->getSusceptible() * eventRate(g->getEvent()->getTime(),acquisition,0,s)) + 
			(s->getLatent() == 0 ? 0 : s->getLatent() * eventRate(g->getEvent()->getTime(),progression,0,s)) + 
			(s->getColonized() == 0 ? 0 : s->getColonized() * eventRate(g->getEvent()->getTime(),clearance,0,s))
		);
	}

	virtual inline void initCounts()
	{
		for (int i=0; i<n; i++)
		{
			shapepar[i] = priorshape[i];
			ratepar[i] = priorrate[i];
		}
	}

	virtual inline void count(HistoryLink *h)
	{
		shapepar[eventIndex(h->getEvent()->getType())] += 1;
	}

	virtual inline void countGap(HistoryLink *g, HistoryLink *h)
	{
		double time = h->getEvent()->getTime() - g->getEvent()->getTime();
		LocationState *s = h->uPrev()->getUState();
		ratepar[0] += time * s->getSusceptible() * acquisitionFactor(s->getColonized(),s->getTotal());
		ratepar[1] += time * s->getLatent();
		ratepar[2] += time * s->getColonized();
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
			set(i,newrates[i]);

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
	
		set(i,value);

		doit[i] = update;
		double n = prin > 1 ? prin : 1;
		priorshape[i] = prival * n;
		priorrate[i] = n;
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
