// infect/PatientState.h
#ifndef ALUN_INFECT_PATIENTSTATE_H
#define ALUN_INFECT_PATIENTSTATE_H

#include "InfectionCoding.h"
#include "State.h"

namespace infect
{
class PatientState : public State, public InfectionCoding
{
protected:

	Unit *u;
	InfectionStatus s;
	int n;

public:

    PatientState(Patient *pp);
    PatientState(Patient *pp, int nstates);

	inline Unit *getUnit() const
	{
		return u;
	}

	inline InfectionStatus infectionStatus() const
	{
		return s;
	}

	virtual int onAbx() const;
	virtual int everAbx() const;
	virtual void copy(State *t);
	virtual void apply(Event *e);
	virtual void unapply(Event *e);
	virtual void write(ostream &os) const;
};
} // namespace infect
#endif // ALUN_INFECT_PATIENTSTATE_H
