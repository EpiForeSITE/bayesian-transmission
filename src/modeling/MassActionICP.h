#ifndef ALUN_MODELING_MASSACTIONICP_H
#define ALUN_MODELING_MASSACTIONICP_H

#include "InColParams.h"
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
			set(0.001,0,0.01);
			setUpdate(1,0,1);
			break;
		case 3:
			set(0.001,0.01,0.01);
			setUpdate(1,1,1);
			break;
		}

		setPriors(1,1,1,1,1,1);
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

		set(newrates[0],newrates[1],newrates[2]);
		delete [] newrates;
	}

// Personal accessors.

	virtual inline void set(double c, double p, double d)
	{
		rates[0] = c;
		rates[1] = p;
		rates[2] = d;
		for (int i=0; i<n; i++)
			logrates[i] = log(rates[i]);
	}

	virtual inline void setPriors(double va, double pna, double vb, double pnb, double vc, double pnc)
	{
		// Value and number of observations pairs)
		double na = pna > 1 ? pna : 1 ;
		double nb = pnb > 1 ? pnb : 1 ;
		double nc = pnc > 1 ? pnc : 1 ;

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

	virtual int nParam()
	{
		return nstates;
	}

	virtual string *paramNames()
	{
		string *res = new string[nstates];

		if (nstates == 3)
		{
			res[0] = "MAICP.acq";
			res[1] = "MAICP.pro";
			res[2] = "MAICP.clr";
		}

		if (nstates == 2)
		{
			res[0] = "MAICP.acq";
			res[1] = "MAICP.clr";
		}

		return res;
	}

	virtual void write (ostream &os)
	{
		char *buffer = new char[100];
		sprintf(buffer,"%12.10f\t",rates[2]);
		os << buffer;
		if (nstates == 3)
		{
			sprintf(buffer,"%12.10f\t",rates[1]);
			os << buffer;
		}
		sprintf(buffer,"%12.10f",rates[0]);
		os << buffer;
		delete[] buffer;
	}
};
#endif // ALUN_MODELING_MASSACTIONICP_H
