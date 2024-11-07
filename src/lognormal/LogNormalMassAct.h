#ifndef ALUN_LOGNORMAL_LOGNORMALMASSACT_H
#define ALUN_LOGNORMAL_LOGNORMALMASSACT_H

#include "LogNormalICP.h"

/*
 Use the LogNormalICP setup to mimic MassActionICP.
*/

class LogNormalMassAct: public LogNormalICP
{
private:

	// value 0 == frequency dependent; 1 == density dependent; 2 == constant;
	int isDensity;

	virtual double logAcquisitionRate(double ncol, double tot)
	{
		double x =  par[0][0] + par[0][2] * log(tot);
		if (par[0][1] > 0)
			x += par[0][1] * log(ncol);
		return x;
	}


public:

	LogNormalMassAct(int k, int isDen, int nmet=10) : LogNormalICP(k,3,1,1,nmet)
	{
		isDensity = isDen;

		setNormal(1,0,log(0.01),1,0,1);
		setNormal(2,0,log(0.01),1,0,1);

		setNormal(0,0,log(0.001),1,0,1);

		switch(isDensity)
		{
		case 2: setNormal(0,1,0,0,0,1);
			setNormal(0,2,0,0,0,1);
			break;

		case 1: setNormal(0,1,1,0,0,1);
			setNormal(0,2,0,0,0,1);
			break;

		case 0: setNormal(0,1,1,0,0,1);
			setNormal(0,2,-1,0,0,1);
			break;
		}

		pnames[1][0] = "LNMA.pro";
		pnames[2][0] = "LNMA.clr";

		pnames[0][0] = "LNMA.base";
		pnames[0][1] = "LNMA.ncol";
		pnames[0][2] = "LNMA.ntot";
	}


// Implement LogNormalICP.

	virtual double logProgressionRate(double time, PatientState *p, LocationState *s)
	{
		return par[1][0];
	}

	virtual double logProgressionGap(double t0, double t1, LocationState *s)
	{
		return ( s->getLatent() == 0 ?  0 : -(t1 - t0) * s->getLatent() * exp(par[1][0]) );
	}

	virtual double logClearanceRate(double time, PatientState *p, LocationState *s)
	{
		return par[2][0];
	}

	virtual double logClearanceGap(double t0, double t1, LocationState *s)
	{
		return ( s->getColonized() == 0 ? 0 :  -(t1 - t0) * s->getColonized() * exp(par[2][0]) );
	}

	virtual inline double logAcquisitionRate(double time, PatientState *p, LocationState *s)
	{
        	return logAcquisitionRate(s->getColonized(),s->getTotal());
	}

	virtual inline double logAcquisitionGap(double t0, double t1, LocationState *s)
	{
		if (s->getSusceptible() > 0)
			return - (t1 - t0) * s->getSusceptible() * exp(logAcquisitionRate(s->getColonized(),s->getTotal()));
		else
			return 0;
	}

	virtual double *acquisitionRates(double time, PatientState *p, LocationState *s)
	{
		double *P = new double[nstates];

                if (nstates == 2)
                {
			P[0] = exp(logAcquisitionRate(s->getColonized(),s->getTotal()));
			P[1] = exp(logAcquisitionRate(1+s->getColonized(),s->getTotal()));
                }

                if (nstates == 3)
                {
			P[0] = exp(logAcquisitionRate(s->getColonized(),s->getTotal()));
			P[1] = exp(logAcquisitionRate(s->getColonized(),s->getTotal()));
			P[2] = exp(logAcquisitionRate(1+s->getColonized(),s->getTotal()));
                }

                return P;
	}
};
#endif // ALUN_LOGNORMAL_LOGNORMALMASSACT_H
