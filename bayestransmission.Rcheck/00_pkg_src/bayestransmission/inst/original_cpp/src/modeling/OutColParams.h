
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

	void resetPQ()
	{
		if (nstates == 2)
		{
			double a = rates[0];
			double b = rates[1];

			sumrates = a+b;

			P[0] = b/sumrates;
			P[1] = a/sumrates;

			Q[0][0] = -a/sumrates;
			Q[0][1] = a/sumrates;
			Q[1][0] = b/sumrates;
			Q[1][1] = -b/sumrates;
		}

		if (nstates == 3)
		{
			double a = rates[0];
			double b = rates[1];
			double c = rates[2];

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

protected:

	virtual inline void set(double *x)
	{
		if (nstates == 2)
			set(x[0],x[1]);
		if (nstates == 3)
			set(x[0],x[1],x[2]);
	}

	virtual inline void set(double a, double b, double c)
	{
		rates[0] = a;
		rates[1] = b;
		rates[2] = c;
		resetPQ();
	}

	virtual inline void set(double a, double b)
	{
		rates[0] = a;
		rates[1] = b;
		resetPQ();
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

		P = cleanAlloc(nstates);
		Q = cleanAlloc(nstates,nstates);
		QQ = cleanAlloc(nstates,nstates);
		I = cleanAlloc(nstates,nstates);
		for (int i=0; i<nstates; i++)
			I[i][i] = 1;

		doit = new int[nstates];

		if (nstates == 3)
			set(1.0,1.0,1.0);
		set(0,1,1,1,1);
		set(1,1,1,1,1);
		set(2,1,1,1,1);
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

	virtual string header()
	{
		stringstream s;
		s << "Out.acquire";
		if (nstates == 3)
                        s << "\t" << "Out.progress";
		s << "\t" << "Out.clear";
		return s.str();
	}

	virtual int getNStates()
	{
		return nstates;
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

	virtual inline void setNMetro(int n)
	{
		nmetro = n;
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
	
		int j = 0;

		if (nstates == 3)
			j = i;

		if (nstates == 2)
		{
			switch(i)
			{
			case 0: j = 0;
				break;

			case 1: return;

			case 2: j = 1;
				break;

			default:
				break;
			}
		}

		rates[j] = value;
		doit[j] = update;
		double n = prin > 1 ? prin : 1;
		priorshape[j] = prival * n;
		priorrate[j] = n;

		resetPQ();
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
