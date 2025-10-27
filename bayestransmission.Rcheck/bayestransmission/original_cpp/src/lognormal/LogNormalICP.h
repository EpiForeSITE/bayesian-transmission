
class LogNormalICP : public InColParams 
{
protected:

	// Parameters are log rates.
	// Acquisition parameters are par[0][*].
	// Progression parameters are par[1][*].
	// Clearance parameters are par[2][*].

	static const int ns = 3;
	int *n;
	double **par;
	double **epar;
	double **primean;
	double **pristdev;
	double **sigmaprop;
	int **doit;
	double tOrigin;
	int nmetro;
	Map *m;

	inline void setNormal(int i, int j, double x)
	{
		par[i][j] = x;
		epar[i][j] = unTransform(i,j);
	}

public:

	LogNormalICP(int k, int napar, int nppar, int ncpar, int nmet = 10) : InColParams(k)
	{
		tOrigin = 0;

		nmetro = nmet;

		m = new Map();
		initCounts();

		n = cleanAllocInt(ns);
		n[0] = napar;
		n[1] = nppar;
		n[2] = ncpar;

		par = new double*[ns];
		epar = new double*[ns];
		primean = new double*[ns];
		pristdev = new double*[ns];
		doit = new int*[ns];
		sigmaprop = new double*[ns];

		for (int i=0; i<ns; i++)
		{
			par[i] = cleanAlloc(n[i]);
			epar[i] = cleanAlloc(n[i]);
			primean[i] = cleanAlloc(n[i]);
			pristdev[i] = cleanAlloc(n[i]);
			doit[i] = cleanAllocInt(n[i]);
			for (int j=0; j<n[i]; j++)
				doit[i][j] = 1; 
			sigmaprop[i] = cleanAlloc(n[i]);
			for (int j=0; j<n[i]; j++)
				sigmaprop[i][j] = 0.1;
		}
	}

	~LogNormalICP()
	{
		delete [] n;

		for (int i=0; i<ns; i++)
		{
			delete [] par[i];
			delete [] epar[i];
			delete [] primean[i];
			delete [] pristdev[i];
			delete [] doit[i];
			delete [] sigmaprop[i];
		}

		delete [] par;
		delete [] epar;
		delete [] primean;
		delete [] pristdev;
		delete [] doit;
		delete [] sigmaprop;
		delete m;
	}

	virtual inline int nParam(int i)
	{
		if (i >= 0 && i <=2)
			return n[i];
		else
			return -1;
	}

// For models that have a time trend.
	
	void setTimeOrigin(double t)
	{
		tOrigin = t;
	}

	double getTimeOrigin()
	{
		return tOrigin;
	}

// Sufficient to implement LogNormalICP.

	virtual double logAcquisitionRate(double time, PatientState *p, LocationState *s) = 0;
	virtual double logAcquisitionGap(double t0, double t1, LocationState *s) = 0;
	virtual double *acquisitionRates(double time, PatientState *p, LocationState *s) = 0;

	virtual double logProgressionRate(double time, PatientState *p, LocationState *s) = 0;
	virtual double logProgressionGap(double t0, double t1, LocationState *s) = 0;

	virtual double logClearanceRate(double time, PatientState *p, LocationState *s) = 0;
	virtual double logClearanceGap(double t0, double t1, LocationState *s) = 0;

// Personal accessors.

	virtual void set(int i, int j, double value, int update, double prival, double priorn)
	{
		setWithLogTransform(i,j,value,update,prival,priorn,0.1);
	}

	virtual double unTransform(int i, int j)
	{
		return exp(par[i][j]);
	}
	
	virtual void setWithLogTransform(int i, int j, double value, int update, double prival, double priorn, double sig = 0.1)
	{
		setNormal(i,j,log(value),update,log(prival),log((priorn+1)/priorn),sig);
	}

	virtual void setWithLogitTransform(int i, int j, double value, int update, double prival, double priorn, double sig = 0.1)
	{
		// For probability parameters.
		//
		// The prior is specified as having mean prival, in (0,1), that was worth priorn observations.
		//
		// A Beta(a,b) prior can be specified with these properties if 
		//	a = prival * priorn, 
		//	b = priorn * (1 - prival)
		//
		// If we transform the variable with a logit transformation
		// the mean and variance are
		//	digamma(a) - digamma(b)
		// 	trigamma(a) + trigamma(b)
		// 
		// Assume that the transformed variable has a Gaussian distribution with this mean and variance.
		// Except, use the raw transform as the Gaussian mean!

		double s2 = trigamma(prival*priorn) + trigamma((1-prival)*priorn);
		setNormal(i,j,logit(value),update,logit(prival),s2,sig);
	}

	virtual void setNormal(int i, int j, double value, int update, double prim, double privar, double sig = 0.1)
	{
//cerr << i << "\t" << j << "\t" << value << "\t" << update << "\t" << prim << "\t" << privar << "\t" << sig << "\n";
		par[i][j] = value;
		epar[i][j] = unTransform(i,j);
		doit[i][j] = update;
		primean[i][j] = prim;
		if (privar < 0)
		{
			cerr << "Error: Cannot set prior variance to be negative\n";
			exit(1);
		}
		pristdev[i][j] = sqrt(privar);
		sigmaprop[i][j] = sig;
	}

	virtual void write (ostream &os)
	{
		char *buffer = new char[100];

		for (int i=0; i<ns; i++)
		{
			if (i == 1 && nstates != 3)
				continue;

			for (int j=0; j<n[i]; j++)
			{
				sprintf(buffer,"%12.10f",epar[i][j]);
				os << buffer;
				if (j != n[i]-1)
					os << "\t";
			}
			if (i != ns-1)
				os << "\t";
		}

		delete[] buffer;
	}

// Implement InColParams.

        virtual inline double eventRate(double time, EventCode c, PatientState *p, LocationState *s)
	{
                switch(c)
                {
                case progression:
                        return exp(logProgressionRate(time,p,s));
                case clearance:
                        return exp(logClearanceRate(time,p,s));
                case acquisition:
                        return exp(logAcquisitionRate(time,p,s));
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
                case progression:
                        return logProgressionRate(h->getEvent()->getTime(),h->pPrev()->getPState(),h->uPrev()->getUState());
                case clearance:
                        return logClearanceRate(h->getEvent()->getTime(),h->pPrev()->getPState(),h->uPrev()->getUState());
                case acquisition:
                        return logAcquisitionRate(h->getEvent()->getTime(),h->pPrev()->getPState(),h->uPrev()->getUState());
                default:
                        return 0;
                }
        }

        virtual inline double logProbGap(HistoryLink *g, HistoryLink *h)
        {
                LocationState *s = h->uPrev()->getUState();
		double t0 = g->getEvent()->getTime();
		double t1 = h->getEvent()->getTime();

		double x = 0;

		x += logProgressionGap(t0,t1,s);
		x += logClearanceGap(t0,t1,s);
		x += logAcquisitionGap(t0,t1,s);

		return x;
        }

	virtual inline void initCounts()
	{
		m->clear();
	}

	virtual inline void count(HistoryLink *h)
	{
	}

	virtual inline void countGap(HistoryLink *g, HistoryLink *h)
	{
		m->put(h,g);
	}

	virtual inline double logpost(Random *r, int max)
	{
		double x = 0;

		if (!max)
			for (int i=0; i<ns; i++)
				for (int j=0; j<n[i]; j++)
					if (doit[i][j])
						x += r->logdnorm(par[i][j],primean[i][j],pristdev[i][j]);

		for (m->init(); m->hasNext(); )
		{
			HistoryLink *h = (HistoryLink *) m->next();
			HistoryLink *g = (HistoryLink *) m->get(h);
			x += logProb(h) + logProbGap(g,h);
		}

		return x;
	}

	virtual inline void update(Random *r, int max)
	{
		double oldlogpost = logpost(r,max);

		for (int its = 0; its < nmetro; its++)
		{
			for (int i=0; i<ns; i++)
				for (int j=0; j<n[i]; j++)
				{
					if (doit[i][j])
					{
						double oldone = par[i][j];
						double newone = oldone + r->rnorm(0,sigmaprop[i][j]);
						setNormal(i,j,newone);
						double newlogpost = logpost(r,max);
	
						if ( (max ? 0 : log(r->runif()) ) <= newlogpost - oldlogpost)
						{
							oldlogpost = newlogpost;
						}
						else
						{
							setNormal(i,j,oldone);
						}
					}
				}
		}
	}
};
