

class AbxLocationState : public CountLocationState, AbxCoding
{
protected: 

	int abxinf;
	int abxlat;

	int everinf;
	int everlat;

public:

	Map *abx;
	Map *ever;

	AbxLocationState(Object *own, int nstates) : CountLocationState(own,nstates)
	{
		abx = new Map();
		abxinf = 0;
		abxlat = 0;
		ever = new Map();
		everinf = 0;
		everlat = 0;
	}

	~AbxLocationState()
	{
		delete abx;
		delete ever;
	}

	inline void clear() 
	{ 
		CountLocationState::clear();
		abx->clear();
		abxinf = 0;
		abxlat = 0;
		ever->clear();
		everinf = 0;
		everlat = 0;
	} 

	virtual void copy(State *s)
	{
		CountLocationState::copy(s);
		AbxLocationState *as = (AbxLocationState *) s;

		abx->clear();
		for (as->abx->init(); as->abx->hasNext(); )
			abx->add(as->abx->next());

		abxinf = as->abxinf;
		abxlat = as->abxlat;

		ever->clear();
		for (as->ever->init(); as->ever->hasNext(); )
			ever->add(as->ever->next());

		everinf = as->everinf;
		everlat = as->everlat;
	}

	virtual inline int onAbx(Patient *p)
	{
		return abx->got(p);
	}

	virtual inline int everAbx(Patient *p)
	{
		return ever->got(p);
	}

	virtual inline int getAbxTotal()
	{
		return abx->size();
	}

	virtual inline int getEverAbxTotal()
	{
		return ever->size();
	}

	virtual inline int getAbxColonized()
	{
		return abxinf;
	}

	virtual inline int getEverAbxColonized()
	{
		return everinf;
	}

	virtual inline int getAbxLatent()
	{
		return abxlat;
	}

	virtual inline int getEverAbxLatent()
	{
		return everlat;
	}

	virtual inline int getAbxSusceptible()
	{
		return abx->size() - abxinf - abxlat;
	}

	virtual inline int getEverAbxSusceptible()
	{
		return ever->size() - everinf - everlat;
	}


	virtual inline int getNoAbxTotal()
	{
		return getTotal() - abx->size();
	}

	virtual inline int getNeverAbxTotal()
	{
		return getTotal() - ever->size();
	}

	virtual inline int getNoAbxColonized()
	{
		return getColonized() - abxinf;
	}

	virtual inline int getNeverAbxColonized()
	{
		return getColonized() - everinf;
	}

	virtual inline int getNoAbxLatent()
	{
		return getLatent() - abxlat;
	}

	virtual inline int getNeverAbxLatent()
	{
		return getLatent() - everlat;
	}

	virtual inline int getNoAbxSusceptible()
	{
		return getSusceptible() - getAbxSusceptible();
	}

	virtual inline int getNeverAbxSusceptible()
	{
		return getSusceptible() - getEverAbxSusceptible();
	}

	virtual void write(ostream &os)
	{
		CountLocationState::write(os);
                os << "\t(" << abx->size() << "," << ever->size() << ")";

/*
		if (ever->size() > getTotal())
			os << "!!!";

                os << "\t" << " [";
                for (abx->init(); abx->hasNext(); )
                       os << abx->next() << " ";
                os << "]";
                os << "\t" << " [";
                for (ever->init(); ever->hasNext(); )
                       os << ever->next() << " ";
                os << "]";
*/
	}

	virtual void apply(Event *e)
	{
		CountLocationState::apply(e);

		if (!ownerWantsEvent(e))
			return;

		Patient *p = e->getPatient();

		// sysabx is a single system wide set of patient on abx.
		// This will only work properly when initalizing the 
		// system when the list of events is run through in 
		// time order.
		// The events here that depend on sysabx cannot
		// be applied or unapplied in the MCMC process.
		// Similarly for syseverabx the set of patients who have
		// ever been on abx;

		int ponabx = sysabx->got(p);
		int pever = syseverabx->got(p);

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
			if (ponabx)
				abx->add(p);
			if (pever)
				ever->add(p);
			break;
		case discharge:
			abx->remove(p);
			ever->remove(p);
			break;
		case abxon:
			ever->add(p);
		case abxoff:
			if (ponabx)
				abx->add(p);
			else
				abx->remove(p);
			break;

		default:
			break;
		}
			
		if (ever->got(p))
		{
			if (nStates() == 2)
			{
				switch(e->getType())
				{
				case acquisition:
					everinf++;
					break;
				case clearance:
					everinf--;
					break;
				default: 
					break;
				}
			}
	
			if (nStates() == 3)
			{
				switch(e->getType())
				{
				case acquisition:
					everlat++;
					break;
				case progression:
					everlat--;
					everinf++;
					break;
				case clearance:
					everinf--;
					break;
				default:
					break;
				}
			}
		}

		if (abx->got(p))
		{
			if (nStates() == 2)
			{
				switch(e->getType())
				{
				case acquisition:
					abxinf++;
					break;
				case clearance:
					abxinf--;
					break;
				default: 
					break;
				}
			}
	
			if (nStates() == 3)
			{
				switch(e->getType())
				{
				case acquisition:
					abxlat++;
					break;
				case progression:
					abxlat--;
					abxinf++;
					break;
				case clearance:
					abxinf--;
					break;
				default:
					break;
				}
			}
		}
	}

	virtual void unapply(Event *e)
	{
		CountLocationState::unapply(e);

		if (!ownerWantsEvent(e))
			return;

		if (ever->got(e->getPatient()))
		{
			if (nStates() == 2)
			{
				switch(e->getType())
				{
				case acquisition:
					everinf--;
					break;
				case clearance:
					everinf++;
					break;
				default:
					break;
				}
			}
	
			if (nStates() == 3)
			{
				switch(e->getType())
				{
				case acquisition:
					everlat--;
					break;
				case progression:
					everinf--;
					everlat++;
					break;
				case clearance:
					everinf++;
					break;
				default:
					break;
				}
			}
		}

		if (abx->got(e->getPatient()))
		{
			if (nStates() == 2)
			{
				switch(e->getType())
				{
				case acquisition:
					abxinf--;
					break;
				case clearance:
					abxinf++;
					break;
				default:
					break;
				}
			}
	
			if (nStates() == 3)
			{
				switch(e->getType())
				{
				case acquisition:
					abxlat--;
					break;
				case progression:
					abxinf--;
					abxlat++;
					break;
				case clearance:
					abxinf++;
					break;
				default:
					break;
				}
			}
		}
	}
};
