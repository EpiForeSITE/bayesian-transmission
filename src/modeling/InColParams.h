#ifndef ALUN_MODELING_INCOLPARAMS_H
#define ALUN_MODELING_INCOLPARAMS_H

#include "Parameters.h"

class InColParams : public Parameters
{
protected:

	int nstates;

public:

	InColParams(int nst)
	{
		nstates = nst;
	}

	virtual int getNStates()
	{
		return nstates;
	}

	virtual double *acquisitionRates(double time, PatientState *p, LocationState *s) = 0;

	virtual double eventRate(double time, EventCode c, PatientState *p, LocationState *s) = 0;

	virtual double **rateMatrix(double time, PatientState *p, LocationState *u) = 0;
};
#endif //ALUN_MODELING_INCOLPARAMS_H
