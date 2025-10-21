
class CountLocationState : public LocationState
{
private:

	int tot;
	int inf;
	int lat;

public:

	CountLocationState(Object *own, int nstates = 0) : LocationState(own,nstates)
	{
		tot = 0;
		inf = 0;
		lat = 0;
	}

	virtual inline void clear() 
	{ 
		tot = 0; 
		inf = 0; 
		lat = 0;
	} 

	virtual inline int getTotal() 
	{ 
		return tot; 
	}

	virtual inline int getColonized() 
	{ 
		return inf; 
	}

	virtual inline int getLatent() 
	{ 
		return lat; 
	}

	virtual inline int getSusceptible() 
	{ 
		return tot - inf - lat; 
	}

	virtual void copy(State *s)
	{
		CountLocationState *cs = (CountLocationState *)s;
		tot = cs->tot;
		lat = cs->lat;
		inf = cs->inf;
	//	n = cs->n;
	}

	virtual void apply(Event *e)
	{
		if (!ownerWantsEvent(e))
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
			tot++;
			break;

		case discharge:
			tot--;
			break;

		default:
			break;
		}
		
		if (nStates() == 2)
		{
			switch(e->getType())
			{
			case acquisition:
				inf++;
				break;
	
			case clearance:
				inf--;
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
				lat++;
				break;
	
			case progression:
				lat--;
				inf++;
				break;
	
			case clearance:
				inf--;
				break;
	
			default:
				break;
			}
		}
	}

	virtual void unapply(Event *e)
	{
		if (!ownerWantsEvent(e))
			return;

		if (nStates() == 2)
		{
			switch(e->getType())
			{
			case acquisition:
				inf--;
				break;
	
			case clearance:
				inf++;
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
				lat--;
				break;
	
			case progression:
				lat++;
				inf--;
				break;
	
			case clearance:
				inf++;
				break;
	
			default:
				break;
			}
		}
	}

	virtual void write(ostream &os)
	{
		os << getOwner() ;
		os << " (" << getSusceptible() << "+" << getLatent() << "+" << getColonized() << "=" << getTotal() << ")";
	}
};
