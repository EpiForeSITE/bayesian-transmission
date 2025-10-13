#ifndef ALUN_MODELING_INCOLPARAMS_H
#define ALUN_MODELING_INCOLPARAMS_H

#include "Parameters.h"

namespace models {

class InColParams : public Parameters
{
protected:

	int nstates;

public:
    virtual inline string className() const override {return "InColParams";}

	InColParams(int nst) : Parameters(), nstates(nst) {}

	virtual int getNStates() const override{return nstates;}

	virtual double* acquisitionRates(double time, infect::PatientState *p, infect::LocationState *s) const = 0;

	virtual double eventRate(double time, EventCode c, infect::PatientState *p, infect::LocationState *s) const = 0;

	virtual double** rateMatrix(double time, infect::PatientState *p, infect::LocationState *u) const = 0;
};

} // namespace models

#endif //ALUN_MODELING_INCOLPARAMS_H
