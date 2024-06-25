
class TestParamsAbx : public TestParams //public Parameters
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
		
		set(0,0,0,0.80);
		set(1,0,0,0.80);
		setUpdate(0,0,0,1);
		setUpdate(1,0,0,0);
		setPriors(0,1,1,1,1,1,1);
		setPriors(1,1,1,1,1,1,1);
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
				
		set(0,newpos[0][0],newpos[1][0],newpos[2][0]);
		set(1,newpos[0][1],newpos[1][1],newpos[2][1]);

		cleanFree(&newpos,l);
	}

// Personal accessors.

	virtual inline void set(int onabx, double punc, double plat, double pcol)
	{
		probs[0][onabx][0] = 1-punc;
		probs[0][onabx][1] = punc;
		probs[1][onabx][0] = 1-plat;
		probs[1][onabx][1] = plat;
		probs[2][onabx][0] = 1-pcol;
		probs[2][onabx][1] = pcol;

		for (int i=0; i<l; i++)
			for (int j=0; j<m; j++)
				for (int k=0; k<n; k++)
					logprobs[i][j][k] = log(probs[i][j][k]);
	}

	virtual inline void setPriors(int onabx, double a, double b, double c, double d, double e, double f)
	{
		// Probs determined by relative values of pairs. Equivalent #obs found by summing.
		priors[0][onabx][0] = a;
		priors[0][onabx][1] = b;
		priors[1][onabx][0] = c;
		priors[1][onabx][1] = d;
		priors[2][onabx][0] = e;
		priors[2][onabx][1] = f;
	}

	virtual inline void setUpdate(int onabx, int unc, int lat, int col)
	{
		doit[0][onabx] = unc;
		doit[1][onabx] = lat;
		doit[2][onabx] = col;
	}

	virtual int nParams()
	{
		return 2*nstates;
	}

	virtual string *paramNames()
	{
		string *res = new string[2*nstates];

		if (nstates == 3)
		{
			res[0] = "ATest.P(+|unc-)";
			res[1] = "ATest.P(+|lat-)";
			res[2] = "ATest.P(+|col-)";
			res[3] = "ATest.P(+|unc+)";
			res[4] = "ATest.P(+|lat+)";
			res[5] = "ATest.P(+|col+)";
		}
	
		if (nstates == 2)
		{
			res[0] = "ATest.P(+|unc-)";
			res[1] = "ATest.P(+|col-)";
			res[2] = "ATest.P(+|unc+)";
			res[3] = "ATest.P(+|col+)";
		}

		return res;
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
