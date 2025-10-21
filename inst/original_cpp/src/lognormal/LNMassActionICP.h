
/*
	This implements LogNormalICP but bypasses the log Normal Metropolis
	updating scheme by using the same Gibbs sampler that 
	modeling::MassActionICP does.

	The Metropolis scheme can be reinstated by commenting out
	the updating code from this class.
		
	The results using Gibbs and Metropolis will not be identical
	but should be stochastially equivalent, modulo mixing properties.
*/

class LNMassActionICP : public LogNormalICP 
{
private:

	int isDensity;
	double *ratepar;
	double *shapepar;

public:

	LNMassActionICP(int k, int isdens, int nmetro=10) : LogNormalICP(k,1,1,1,nmetro)
	{
		isDensity = isdens;
		shapepar = new double[ns];
		ratepar = new double[ns];
	}

	~LNMassActionICP()
	{
		delete [] shapepar;
		delete [] ratepar;
	}

// Implement parameters. 

	virtual string header()
	{
		stringstream s;
		s <<  "MAICP.acq";
		if (nstates == 3)
			s << "\t" << "MAICP.pro";
		s << "\t" << "MAICP.clr";
		return s.str();
	}

// Implement LogNormalICP

	inline double acqRate(double c, double t, double s)
	{
		double base = epar[0][0];
		double infectiousness = (isDensity == 2 ? 1 : (isDensity == 1 ? c : (t>0 ? c/t : 0) ));
		double susceptibility = s;

		return base * infectiousness * susceptibility;
	}

	virtual inline double logAcquisitionRate(double time, PatientState *p, LocationState *s)
	{
		return log(acqRate(s->getColonized(),s->getTotal(),1));
	}

	virtual inline double logAcquisitionGap(double t0, double t1, LocationState *s)
	{
		return -(t1 - t0) * acqRate(s->getColonized(),s->getTotal(),s->getSusceptible());
	}

	virtual double *acquisitionRates(double time, PatientState *p, LocationState *s)
	{
		double *P = new double[nstates];

		if (nstates == 2)
		{
			P[0] = acqRate(s->getColonized(),s->getTotal(),1);
			P[1] = acqRate(1+s->getColonized(),s->getTotal(),1);
		}

		if (nstates == 3)
		{
			P[0] = acqRate(s->getColonized(),s->getTotal(),1);
			P[1] = acqRate(s->getColonized(),s->getTotal(),1);
			P[2] = acqRate(1+s->getColonized(),s->getTotal(),1);
		}

		return P;
	}

	virtual double logProgressionRate(double time, PatientState *p, LocationState *s)
	{
		return par[1][0];
	}

	virtual double logProgressionGap(double t0, double t1, LocationState *s)
	{
		return -(t1 - t0) * epar[1][0] * s->getLatent();
	}

	virtual double logClearanceRate(double time, PatientState *p, LocationState *s)
	{
		return par[2][0];
	}

	virtual double logClearanceGap(double t0, double t1, LocationState *s)
	{
		return -(t1 - t0) * epar[2][0] * s->getColonized();
	}

/*
	Comment this section for Metropolis sampling.
	Uncomment this section for Gibbs sampling.
*/

/*
// HERE
	virtual inline void initCounts()
	{
		for (int i=0; i<ns; i++)
		{
			shapepar[i] = 1;
			ratepar[i] = 1;
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

		double x = 1;
		if (isDensity == 1)
			x = s->getColonized();
		if (isDensity == 0)
			x = s->getTotal() > 0 ? s->getColonized() / (double) s->getTotal() : 0;

		ratepar[0] += time * s->getSusceptible() * x;
		ratepar[1] += time * s->getLatent();
		ratepar[2] += time * s->getColonized();
	}

	virtual inline void update(Random *r, int max)
	{
		if (max)
			for (int i=0; i<ns; i++)
			{
				if (doit[i][0])
					setNormal(i,0,log((shapepar[i]-1)/ratepar[i]));
			}
		else
			for (int i=0; i<ns; i++)
			{
				if (doit[i][0])
					setNormal(i,0,log(r->rgamma(shapepar[i],ratepar[i])));
			}
	}
// TO HERE
*/
};
