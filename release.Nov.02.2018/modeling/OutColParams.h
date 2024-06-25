
class OutColParams : public Parameters
{
private:

	int nstates;
	int nmetro;

	Map *admits;
	int countscount;

	double *rates;
	double *priorshape;
	double *priorrate;
	double *P;
	double **I;
	double **Q;
	double **QQ;

	int *doit;

	double sumrates;
	complex<double> l2;
	complex<double> l3;

	virtual inline int stateIndex(InfectionStatus s)
	{
		if (nstates == 2)
		{
			switch(s)
			{
			case uncolonized: return 0;
			case colonized: return 1;
			default:
				return -1;
			}
		}
		if (nstates == 3)
		{
			switch(s)
			{
			case uncolonized: return 0;
			case latent: return 1;
			case colonized: return 2;
			default:
				return -1;
			}
		}
		return -1;
	}

	double logpost(Random *r, int x)
	{ 
		// log prior.
		double f = 0;
		for (int i=0; i<nstates; i++)
		{
			f += r->logdgamma(rates[i],priorshape[i],priorrate[i]);
			f += (1-x)*log(rates[i]);
		}

		double z = 0;
		double n = 0;
		admits->init();
		for (Map *p = admits; p->hasNext(); )
		{
			n += 1;
			HistoryLink *h = (HistoryLink *) p->next();
			if (h->getPState()->infectionStatus() == colonized)
				z += 1;
		}

		admits->init();
		for (Map *p = admits; p->hasNext(); )
		{
			f += logProb((HistoryLink *)p->next());
		}

		return f;
	}
	
	virtual double prob(int i, int j, double t)
	{
	       	if (j < 0)
		       	return 0;
	       	if (i < 0)
		       	return P[j];

		if (nstates == 2)
		{
			return P[j] + -exp(-t*sumrates) * Q[i][j];
		}

		if (nstates == 3)
		{
			complex<double> el2t = exp(l2*t)-1.0;
			complex<double> el3t = exp(l3*t)-1.0;
			double a1 = real( el2t/l2 - (l3*el2t-l2*el3t) / ((l2-l3)*l3) );
			double a2 = real( (l3*el2t - l2*el3t) / ((l2-l3)*l3*l2) );
			return I[i][j] + a1 * Q[i][j] + a2 * QQ[i][j];
		}

		return 0;
	}

	virtual void update(Random *r, int nsteps, int max)
	{
		double f = logpost(r,max);
		double *oldrates = new double[nstates];
		double *newrates = new double[nstates];

		for (int k=0; k<nsteps; k++)
		{
			double oldf = f;

			for (int i=0; i<nstates; i++)
			{
				oldrates[i] = rates[i];	
				if (doit[i])
					newrates[i] = exp(log(rates[i])+r->rnorm(0,1.0));
				else
					newrates[i] = oldrates[i];
			}
			set(newrates);

			f = logpost(r,max);

			if ( (max? 0 : log(r->runif())) > f-oldf)
			{
				// Reject
				set(oldrates);
				f = oldf;
			}
		}

		delete[] oldrates;
		delete[] newrates;
	}

public:

	OutColParams(int nst, int nmet) 
	{
		admits = new Map();
		countscount = 0;

		nstates = nst;
		nmetro = nmet;

		rates = new double[nstates];
		priorshape = new double[nstates];
		priorrate = new double[nstates];

		P = new double[nstates];
		Q = cleanAlloc(nstates,nstates);
		QQ = cleanAlloc(nstates,nstates);
		I = cleanAlloc(nstates,nstates);
		for (int i=0; i<nstates; i++)
			I[i][i] = 1;

		doit = new int[nstates];

		if (nstates == 2)
		{
			set(1,1);
			setUpdate(1,1);
			setPriors(1,1,1,1);
		}

		if (nstates == 3)
		{
			set(1,1,1);
			setUpdate(1,1,1);
			setPriors(1,1,1,1,1,1);
		}
	}

	~OutColParams()
	{
		delete admits;

		cleanFree(&I,nstates);
		cleanFree(&Q,nstates);
		cleanFree(&QQ,nstates);
		delete [] P;
		delete [] rates;
		delete [] priorshape;
		delete [] priorrate;
		delete [] doit;
	}

	virtual inline double transitionProb(InfectionStatus p, InfectionStatus c, double t)
	{
		return prob(stateIndex(p),stateIndex(c),t);
	}

	double *equilibriumProbs()
	{
		double *p = new double[nstates];

		if (nstates == 2)
		{
			p[0] = P[0];
			p[1] = P[1];
		}

		if (nstates == 3)
		{
			p[0] = P[0];
			p[1] = P[1];
			p[2] = P[2];
		}

		return p;
	}

	double **rateMatrix()
	{
		double **q = cleanAlloc(nstates,nstates);

		if (nstates == 2)
		{
			q[0][1] = rates[0];
			q[0][0] = -q[0][1];
			q[1][0] = rates[1];
			q[1][1] = -q[1][0];
		}

		if (nstates == 3)
		{
			q[0][1] = rates[0];
			q[0][0] = -q[0][1];
			q[1][2] = rates[1];
			q[1][1] = -q[1][2];
			q[2][0] = rates[2];
			q[2][2] = -q[2][0];
		}

		return q;
	}

// Implement Parameters.

	virtual inline double logProb(HistoryLink *h)
	{
		if (!h->getEvent()->isAdmission())
			return 0;

		PatientState *prev = 0;
		double time = 0;
		PatientState *cur = h->getPState();
		
		if (h->pPrev() != 0)
		{
			prev = h->pPrev()->getPState();
			time = h->getEvent()->getTime() - h->pPrev()->getEvent()->getTime();
		}

		int i = -1;
		int j = -1;
		if (prev != 0)
			i = stateIndex(prev->infectionStatus());
		if (cur != 0)
			j = stateIndex(cur->infectionStatus());
		return log(prob(i,j,time));
	}
	
	virtual inline void initCounts()
	{
		countscount++;
	}

	virtual inline void count(HistoryLink *h)
	{
		if (countscount == 1)
			if (h->getEvent()->isAdmission())
				admits->add(h);
	}

	virtual inline void update(Random *r, int max)
	{
		update(r,nmetro,max);
	}

// Personal accessors.

	virtual inline void set(double *x)
	{
		if (nstates == 2)
			set(x[0],x[1]);
		if (nstates == 3)
			set(x[0],x[1],x[2]);
	}

	virtual inline void set(double a, double b)
	{
		rates[0] = a;
		rates[1] = b;

		sumrates = a+b;

		P[0] = b/sumrates;
		P[1] = a/sumrates;

		Q[0][0] = -a/sumrates;
		Q[0][1] = a/sumrates;
		Q[1][0] = b/sumrates;
		Q[1][1] = -b/sumrates;
	}

	virtual inline void set(double a, double b, double c)
	{
		rates[0] = a;
		rates[1] = b;
		rates[2] = c;

		double m = a*b + b*c + c*a;
		double l = a + b + c;
		double d = l*l - 4*m;

		P[0] = b*c/m;
		P[1] = c*a/m;
		P[2] = a*b/m;

		Q[0][0] = -a;
		Q[0][1] = a;
		Q[0][2] = 0;
		Q[1][0] = 0;
		Q[1][1] = -b;
		Q[1][2] = b;
		Q[2][0] = c;
		Q[2][1] = 0;
		Q[2][2] = -c;

		for (int i=0; i<nstates; i++)
			for (int j=0; j<nstates; j++)
			{
				QQ[i][j] = 0;
				for (int k=0; k<nstates; k++)
					QQ[i][j] += Q[i][k]*Q[k][j];
			}

		complex<double> rd = d;
		rd = sqrt(rd);

		l2 = (-l + rd)/2.0;
		l3 = (-l - rd)/2.0;
	}

	virtual inline void setUpdate(int a, int b)
	{
		if (nstates == 2)
		{
			doit[0] = a;
			doit[1] = b;
		}
	}

	virtual inline void setUpdate(int a, int b, int c)
	{
		if (nstates == 3)
		{
			doit[0] = a;
			doit[1] = b;
			doit[2] = c;
		}
	}

	virtual inline void setNMetro(int n)
	{
		nmetro = n;
	}

	virtual inline void setPriors(double va, double pna, double vb, double pnb)
	{
		// Specified as (value, equivalent observations) pairs.
		double na = pna > 1 ? pna : 1;
		double nb = pnb > 1 ? pnb : 1;

		priorshape[0] = va*na;
		priorrate[0] = na;
		priorshape[1] = vb*nb;
		priorrate[1] = nb;
	}

	virtual inline void setPriors(double va, double pna, double vb, double pnb, double vc, double pnc)
	{
		// Specified as (value, equivalent observations) pairs.
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

	virtual inline int nParam()
	{
		return nstates;
	}

	virtual string *paramNames()
	{
		string *res = new string[nstates];
		
		if (nstates == 3)
		{
			res[0] = "Out.acq";
			res[1] = "Out.pro";
			res[2] = "Out.clr";
		}
		
		if (nstates == 2)
		{
			res[0] = "Out.acq";
			res[1] = "Out.clr";
		}

		return res;
	}

	void write(ostream &os)
	{
		char *buffer = new char[100];
		for (int i=0; i<nstates; i++)
		{
			sprintf(buffer,"%12.10f",rates[i]);
			os << buffer;
			if (i < nstates-1)
				os << "\t";
		}
		delete [] buffer;
	}
};
