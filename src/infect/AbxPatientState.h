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

	AbxPatientState(Patient *pp, int nstates) : PatientState(pp, nstates)
	{
		abc = 0;
		ever = 0;
	}

	virtual inline int onAbx() const
	{
		return abc;
	}

	virtual inline int everAbx() const
	{
		return ever;
	}

	virtual void copy(State *t)
	{
		PatientState::copy(t);
		abc = ((AbxPatientState *)t)->abc;
		ever = ((AbxPatientState *)t)->ever;
	}

	virtual void write(ostream &os)
	{
		PatientState::write(os);
	//	os << " \t" << abxCodeString( abc > 0 ? onabx : offabx);
		os << " \t" << "abx(" << abc << "," << ever << ")";
	}

	virtual void apply(Event *e)
	{
		if (getOwner() != e->getPatient())
			return;

		PatientState::apply(e);

		switch(e->getType())
		{
		case abxon:
			abc++;
			ever = 1;
			break;

		case abxoff:
			abc--;
			break;

		default:
			break;
		}

		if (abc < 0)
			abc = 0;

		if (abc)
			sysabx->add(e->getPatient());
		else
			sysabx->remove(e->getPatient());

		if (ever)
			syseverabx->add(e->getPatient());

	}
};
#endif // ALUN_INFECT_ABXPATIENTSTATE_H
