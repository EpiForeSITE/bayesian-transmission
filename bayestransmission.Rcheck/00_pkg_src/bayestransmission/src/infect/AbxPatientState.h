// infect/AbxPatientState.h
#ifndef ALUN_INFECT_ABXPATIENTSTATE_H
#define ALUN_INFECT_ABXPATIENTSTATE_H

#include "PatientState.h"
#include "AbxCoding.h"

class AbxPatientState : public PatientState, public AbxCoding
{
protected:

	int abc;
	int ever;

public:

	AbxPatientState(Patient *pp, int nstates);
	virtual int onAbx() const override;
	virtual int everAbx() const override;
	virtual void copy(State *t);
	virtual void apply(Event *e);
	virtual void write(ostream &os) const;
};
#endif // ALUN_INFECT_ABXPATIENTSTATE_H
