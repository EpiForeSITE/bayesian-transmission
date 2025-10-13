#ifndef ALUN_LOGNORMAL_MULTIUNITABXICP_H
#define ALUN_LOGNORMAL_MULTIUNITABXICP_H

#include "LogNormalAbxICP.h"

class MultiUnitAbxICP: public LogNormalAbxICP
{
private:

	double acqRate(int unit, int onabx, double ncolabx, double ncol, double tot) const;

	Map *units;

	int index(Object *u) const;

public:
    virtual inline string className() const override {return "MultiUnitAbxICP";}
	MultiUnitAbxICP(List *u, int nst, int isDensity, int nmet);
	virtual void setUnit(int i, Object *u, double value, int update, double prival, double priorn, double sig);

// Implement LogNormalICP.
	virtual double logAcquisitionRate(double time, PatientState *p, LocationState *ls) const override;
	virtual double logAcquisitionGap(double u, double v, LocationState *ls) const override;
	virtual double* acquisitionRates(double time, PatientState *p, LocationState *ls) const override;
};
#endif // ALUN_LOGNORMAL_MULTIUNITABXICP_H
