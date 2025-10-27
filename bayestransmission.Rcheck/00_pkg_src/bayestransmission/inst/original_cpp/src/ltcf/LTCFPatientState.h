
class LTCFPatientState : public PatientState, public LTCFCoding
{
protected: 

	int isol;
	int ever;

public:

	LTCFPatientState(Patient *pp, int nstates) : PatientState(pp, nstates)
	{
		isol = 0;
		ever = 0;
	}

/*
	virtual inline int inIsol()
	{
		return isol;
	}

	virtual inline int everIsol()
	{
		return ever;
	}
*/

	virtual void copy(State *t)
	{
		PatientState::copy(t);
		isol = ((LTCFPatientState *)t)->isol;
		ever = ((LTCFPatientState *)t)->ever;
	}

	virtual void write(ostream &os)
	{
		PatientState::write(os);
		os << " \t" << "isol(" << isol << "," << ever << ")";
	}

	virtual void apply(Event *e)
	{
		if (getOwner() != e->getPatient())
			return;
		
		PatientState::apply(e);

		switch(e->getType())
		{
		case isolon:
			isol++;
			ever = 1;
			break;

		case isoloff:
			isol--;
			break;

		default:
			break;
		}

		if (isol < 0) 
			isol = 0;

		if (isol)
			sysisol->add(e->getPatient());
		else
			sysisol->remove(e->getPatient());

		if (ever)
			syseverisol->add(e->getPatient());
	}
};
