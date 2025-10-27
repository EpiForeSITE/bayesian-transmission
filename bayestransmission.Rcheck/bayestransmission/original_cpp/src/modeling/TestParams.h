
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

	virtual inline void set(int i, double value)
	{
		probs[i][0] = 1-value;
		probs[i][1] = value;
		logprobs[i][0] = log(probs[i][0]);
		logprobs[i][1] = log(probs[i][1]);
	}

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
		
		set(0,0,0,0.5,2);
		set(1,0,0,0.5,2);
		set(2,0.8,1,0.5,2);

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

	virtual string header()
	{
		stringstream s;
		s <<  "Test.Punc";
		if (nstates == 3)
			s << "\t" << "Test.Plat";
		s << "\t" << "Test.Pcol";
		return s.str();
	}

	virtual int getNStates()
	{
		return nstates;
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
			{
				if (doit[i])
				{
					double mode = (counts[i][1]-1) / (counts[i][0]+counts[i][1]-2);
					if (counts[i][1] < 1)
						mode = 0;
					if (counts[i][0] < 1)
						mode = 1;
					newpos[i] = mode;
				}
				else
				{
					newpos[i] = probs[i][1];
				}
			}
		}
		else
		{
			for (int i=0; i<n; i++)
			{
//cerr << i <<"\t" << "Beta\t" << counts[i][1] << "\t" << counts[i][0];
				newpos[i] = ( doit[i] ? r->rbeta(counts[i][1],counts[i][0]) : probs[i][1] );
//cerr << "\t" << newpos[i] << "\n";
			}
//cerr << "\n";
		}

		for (int i=0; i<n; i++)
			set(i,newpos[i]);

		delete [] newpos;
	}

// Personal accessors.

	// Set value, update and Beta prior.
	virtual void set(int i, double value, int update, double prival, double prin)
	{
		if (value < 0 || value > 1)
		{
			cerr << "Error: can't set probablilty value outside of (0,1)\t" << value << "\n";
			exit(1);
		}
		if (prival < 0 || prival > 1)
		{
			cerr << "Error: can't set probablilty value outside of (0,1)\t" << prival << "\n";
			exit(1);
		}
		if (prin < 0)
		{
			cerr << "Error: can't set prior observations less than 0\t" << prin << "\n";
			exit(1);
		}

		set(i,value);
		//set(i,0.5);

		doit[i] = (update != 0);

		priors[i][0] = (1-prival) * prin;
		priors[i][1] = prival * prin;
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
