
class SetLocationState : public LocationState, public InfectionCoding
{
protected: 

	Map *pat;

public:

	SetLocationState(Object *own, int ns = 0) : LocationState(own,ns)
	{
		pat = new Map();
	}

	~SetLocationState()
	{
		delete pat;
	}

	inline void clear() 
	{ 
		pat->clear();
	} 

	inline Map *getPatients()
	{
		pat->init();
		return pat;
	}

	virtual void copy(State *ss)
	{
		SetLocationState *s = (SetLocationState *) ss;
		pat->clear();
		for (Map *p = s->getPatients(); p->hasNext(); )
			pat->add(p->next());
	}

	virtual void apply(Event *e)
	{
		Patient *p = e->getPatient();
		if (p == 0)
			return;

		switch(e->getType())
		{
		case insitu:
		case insitu0:
		case insitu1:
		case insitu2:
		case admission:
		case admission0:
		case admission1:
		case admission2:
			pat->add(p);
			break;

		case discharge:
			pat->remove(p);
			break;

		default:
			break;
		}
	}

	virtual void unapply(Event *e)
	{
	}

	void write(ostream &os)
	{
		os << "\t(" << pat->size() << ")";
		os << "\t" << " [";
		for (Map *p = getPatients(); p->hasNext(); )
			os << p->next() << " ";
		os << "]";
	}

};
