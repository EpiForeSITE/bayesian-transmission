
class PatientState : public State, public InfectionCoding
{
protected: 

	Unit *u;
	InfectionStatus s;
	int n;

public:
	
	PatientState(Patient *pp) : State(pp)
	{
		u = 0;
		s = uncolonized;
		n = 0;
	}

	PatientState(Patient *pp, int nstates) : State(pp)
	{
		u = 0;
		s = uncolonized;
		n = nstates;
	}

	inline Unit *getUnit()
	{
		return u;
	}

	inline InfectionStatus infectionStatus() 
	{ 
		return s; 
	}

	inline virtual int onAbx()
	{
		return 0;
	}

	virtual void copy(State *t)
	{
		PatientState *p = (PatientState *) t;
		u = p->u;
		s = p->s;
	}

	virtual void apply(Event *e)
	{
		if (getOwner() != e->getPatient())
			return;

		switch(e->getType())
		{
		case admission:
		case admission0:
		case admission1:
		case admission2:
		case insitu:
		case insitu0:
		case insitu1:
		case insitu2:
			u = e->getUnit();
			break;
		case discharge:
			u = 0;
			break;
		default:
			break;
		}

		if (n == 2)
		{
			switch(e->getType())
			{
			case acquisition:
				s = colonized;
				break;
			case clearance:
				s = uncolonized;
				break;
	
			default:
				break;
			}
		}

		if (n == 3)
		{
			switch(e->getType())
			{
			case acquisition:
				s = latent;
				break;
			case progression:
				s = colonized;
				break;
			case clearance:
				s = uncolonized;
				break;
	
			default:
				break;
			}
		}
	}

	virtual void unapply(Event *e)
	{
		if (getOwner() != e->getPatient())
			return;

		if (n == 2)
		{
			switch(e->getType())
			{
			case acquisition:
				s = uncolonized;
				break;
			case clearance:
				s = colonized;
				break;
	
			default:	
				break;
			}
		}

		if (n == 3)
		{
			switch(e->getType())
			{
			case acquisition:
				s = uncolonized;
				break;
			case progression:
				s = latent;
				break;
			case clearance:
				s = colonized;
				break;
	
			default:	
				break;
			}
		}
	}

	virtual void write(ostream &os)
	{
		State::write(os);
		os << "\tUnit: ";
		os << ( u == 0 ? 0 : u->getId());
		os << " \t" << codeString(s);
	}
};
