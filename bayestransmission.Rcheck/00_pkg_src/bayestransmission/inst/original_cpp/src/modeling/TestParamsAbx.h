
class TestParamsAbx : public TestParams
{
protected:

	int nstates;
	int l;
	int m;
	int n;

	// l x m x n array with probs[i][j][k] = P(test result == k | colonization status = i, abx status = j)

	double ***probs;
	double ***logprobs;
	double ***counts;
	double ***priors;
	int **doit;

	int useabx;

	virtual inline void set(int i, int j, double value)
	{
		probs[i][j][0] = 1-value;
		probs[i][j][1] = value;
		logprobs[i][j][0] = log(probs[i][j][0]);
		logprobs[i][j][1] = log(probs[i][j][1]);
	}

public:

	TestParamsAbx(int nst, int abx) : TestParams(nst)
	{
		useabx = abx;

		nstates = nst;
		l = 3;
		m = 2;
		n = 2;

		probs = cleanAlloc(l,m,n);
		logprobs = cleanAlloc(l,m,n);
		counts = cleanAlloc(l,m,n);
		priors = cleanAlloc(l,m,n);
		doit = cleanAllocInt(l,m);

		set(0,0,0,0,0.5,2);
		set(1,0,0,0,0.5,2);
		set(2,0,0.8,1,0.5,2);

		set(0,1,0,0,0.5,2);
		set(1,1,0,0,0.5,2);
		set(2,1,0.8,1,0.5,2);

		initCounts();
	}

	~TestParamsAbx()
	{
		cleanFree(&probs,l,m);
		cleanFree(&logprobs,l,m);
		cleanFree(&counts,l,m);
		cleanFree(&priors,l,m);
		cleanFree(&doit,l);
	}

	virtual string header()
	{
		stringstream s;
		
		if (nstates == 3)
		{
			s << "ATest.Puncnoabx";
			s << "\t" << "ATest.Platnoabx";
			s << "\t" << "ATest.Pcolnoabx";
			s << "\t" << "ATest.Puncabx";
			s << "\t" << "ATest.Platabx";
			s << "\t" << "ATest.Pcolabx";
		}

		if (nstates == 2)
		{
			s << "ATest.Puncnoabx";
			s << "\t" << "ATest.Pcolnoabx";
			s << "\t" << "ATest.Puncabx";
			s << "\t" << "ATest.Pcolabx";
		}

		return s.str();
	}

	inline void setUseAbx(int i)
	{
		useabx = i;
	}

	inline int getUseAbx()
	{
		return useabx;
	}

	virtual inline double eventProb(InfectionStatus s, int onabx, EventCode e)
	{
		int i = stateIndex(s);
		int j = ( useabx && onabx ? 1 : 0) ;
		int k = testResultIndex(e);
		return ( i < 0 || k < 0 ? 0 : probs[i][j][k] );
	}

	virtual double *resultProbs(int onabx, EventCode e)
	{
		double *P = cleanAlloc(nstates);

		if (nstates == 2)
		{
			P[0] = eventProb(uncolonized,onabx,e);
			P[1] = eventProb(colonized,onabx,e);
		}

		if (nstates == 3)
		{
			P[0] = eventProb(uncolonized,onabx,e);
			P[1] = eventProb(latent,onabx,e);
			P[2] = eventProb(colonized,onabx,e);
		}

		return P;
	}

// Implement Parameters.

	inline virtual double logProb(HistoryLink *h)
	{
		int i = stateIndex(h->getPState()->infectionStatus());
		int j = ( useabx && h->getPState()->onAbx() ? 1 : 0) ;
		int k = testResultIndex(h->getEvent()->getType());

		return ( i < 0 || k < 0 ? 0 : logprobs[i][j][k] );
	}

	inline virtual void initCounts()
	{
		for (int i=0; i<l; i++)
			for (int j=0; j<m; j++)
				for (int k=0; k<n; k++)
					counts[i][j][k] = priors[i][j][k];
	}

	inline virtual void count(HistoryLink *h)
	{
		int i = stateIndex(h->getPState()->infectionStatus());
		int j = ( useabx && h->getPState()->onAbx() ? 1 : 0) ;
		int k = testResultIndex(h->getEvent()->getType());

		if (i >= 0 && k >= 0)
			counts[i][j][k] += 1;
	}

	inline virtual void update(Random *r, int max)
	{
		double **newpos = cleanAlloc(l,m); 

		for (int i=0; i<l; i++)
			for (int j=0; j<m; j++)
			{
				if (doit[i][j])
				{
					if (max)
						newpos[i][j] = (counts[i][j][1]-1) / (counts[i][j][1] + counts[i][j][0]-2);
					else
						newpos[i][j] = r->rbeta(counts[i][j][1],counts[i][j][0]);
				}
				else
					newpos[i][j] = probs[i][j][1];
			}
				
		for (int i=0; i<l; i++)
			for (int j=0; j<m; j++)
				set(i,j,newpos[i][j]);

		cleanFree(&newpos,l);
	}

// Personal accessors.

	// Set value, update, and Beta priors.
	virtual inline void set(int i, int j, double value, int update, double prival, double prin)
	{
		if (value < 0 || value > 1)
		{
			cerr << "Can't set probability value outside (0,1)\t" << value << "\n";
			exit(1);
		}
		if (prival < 0 || prival > 1)
		{
			cerr << "Can't set probability prior value outside (0,1)\t" << prival << "\n";
			exit(1);
		}
		if (prin < 0)
		{
			cerr << "Can't set prior observation count negative\t" << prin << "\n";
			exit(1);
		}

		set(i,j,value);

		doit[i][j] = (update != 0);

		priors[i][j][0] = (1-prival)*prin;
		priors[i][j][1] = prival*prin;
	}

	virtual void write (ostream &os)
	{
		char *buffer = new char[100];

		sprintf(buffer,"%12.10f\t",probs[0][0][1]);
		os << buffer;
		if (nstates == 3)
		{
			sprintf(buffer,"%12.10f\t",probs[1][0][1]);
			os << buffer; 
		}
		sprintf(buffer,"%12.10f",probs[2][0][1]);
		os << buffer;

		if (useabx)
		{
			sprintf(buffer,"\t%12.10f\t",probs[0][1][1]);
			os << buffer;
			if (nstates == 3)
			{
				sprintf(buffer,"%12.10f\t",probs[1][1][1]);
				os << buffer; 
			}
			sprintf(buffer,"%12.10f",probs[2][1][1]);
			os << buffer;
		}

		delete [] buffer;
	}
};
