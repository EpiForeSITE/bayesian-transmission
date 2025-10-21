

class LTCFLocationState : public SetLocationState, LTCFCoding
{
protected:

	Map *isol;
	Map *ever;

public:

	LTCFLocationState(Object *own, int nstates) : SetLocationState(own,nstates)
	{
		isol = new Map();
		ever = new Map();
	}

	~LTCFLocationState()
	{
		delete isol;
		delete ever;
	}

	virtual inline int inIsol(Patient *p)
	{
		return isol->got(p);
	}

	virtual inline int everIsol(Patient *p)
	{
		return ever->got(p);
	}

	virtual inline int getIsolColonized()
	{
		return interSize(isol,col);
	}

	virtual inline int getIsolColonized(int g)
	{
		return interSize(isol,col,g);
	}

	virtual inline int getIsolSusceptible()
	{
		return interSize(isol,sus);
	}

	virtual inline int getIsolSusceptible(int g)
	{
		return interSize(isol,sus,g);
	}

	virtual inline int getEverIsolSusceptible()
	{
		return interSize(ever,sus);
	}

	virtual inline int getEverIsolSusceptible(int g)
	{
		return interSize(ever,sus,g);
	}

// This apply only works when the events are read in in time order.
// Like AbxLocationState.

	virtual void apply(Event *e)
	{
		SetLocationState::apply(e);

		if (!ownerWantsEvent(e))
			return;

		Patient *p = e->getPatient();

		int ponisol = sysisol->got(p);
		int pever = syseverisol->got(p);

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
			if (ponisol)
				isol->add(p);
			if (pever)
				ever->add(p);
			break;
		case discharge:
			isol->remove(p);
			ever->remove(p);
			break;
		case isolon:
			ever->add(p);
		case isoloff:
			if (ponisol)
				isol->add(p);
			else
				isol->remove(p);
			break;

		default:
			break;
		}
	}

	virtual void unapply(Event *e)
	{
		SetLocationState::unapply(e);
	}

	virtual inline void clear() 
	{ 
		SetLocationState::clear();
		isol->clear();
		ever->clear();
	} 

	virtual void copy(State *s)
	{
		SetLocationState::copy(s);
		LTCFLocationState *as = (LTCFLocationState *) s;

		isol->clear();
		for (as->isol->init(); as->isol->hasNext(); )
			isol->add(as->isol->next());

		ever->clear();
		for (as->ever->init(); as->ever->hasNext(); )
			ever->add(as->ever->next());
	}

};
