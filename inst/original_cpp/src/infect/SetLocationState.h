
class SetLocationState : public LocationState, public InfectionCoding
{
protected: 

	Map *pat;
	Map *sus;
	Map *lat;
	Map *col;

	int interSize(Map *a, Map *b)
	{
		Map *x = a;
		Map *y = b;
		if (a->size() > b->size())
		{
			y = a;
			x = b;
		}

		int s = 0;
		for (x->init(); x->hasNext(); )
		{
			Patient *p = (Patient *)x->next();
			if (y->got(p))
				s++;
		}
		return s;
	}

	int interSize(Map *a, Map *b, int g)
	{
		Map *x = a;
		Map *y = b;
		if (a->size() > b->size())
		{
			y = a;
			x = b;
		}

		int s = 0;
		for (x->init(); x->hasNext(); )
		{
			Patient *p = (Patient *)x->next();
			if (p->getGroup() == g && y->got(p))
				s++;
		}
		return s;
	}

	int subsetSize(Map *a, int g)
	{
		int s = 0;
		for (a->init(); a->hasNext(); )
		{
			Patient *p = (Patient *) a->next();
			if (p->getGroup() == g)
				s++;
		}
		return s;
	}

public:

	SetLocationState(Object *own, int ns = 0) : LocationState(own,ns)
	{
		pat = new Map();
		sus = new Map();
		lat = new Map();
		col = new Map();
	}

	~SetLocationState()
	{
		delete pat;
		delete sus;
		delete lat;
		delete col;
	}

	inline void clear() 
	{ 
		pat->clear();
		sus->clear();
		lat->clear();
		col->clear();
	} 

	inline Map *getPatients()
	{
		pat->init();
		return pat;
	}

	virtual int getTotal()
	{
		return pat->size();
	}

	virtual int getTotal(int g)
	{
		return subsetSize(pat,g);
	}

	virtual int getSusceptible()
	{
		return sus->size();
	}

	virtual int getSusceptible(int g)
	{
		return subsetSize(sus,g);
	}

	virtual int getLatent()
	{
		return lat->size();
	}

	virtual int getLatent(int g)
	{
		return subsetSize(lat,g);
	}

	virtual int getColonized()
	{
		return col->size();
	}

	virtual int getColonized(int g)
	{
		return subsetSize(col,g);
	}

	virtual void copy(State *ss)
	{
		SetLocationState *s = (SetLocationState *) ss;

		pat->clear();
		s->pat->init();
		for (Map *p = s->pat; p->hasNext(); )
			pat->add(p->next());

		sus->clear();
		s->sus->init();
		for (Map *p = s->sus; p->hasNext(); )
			sus->add(p->next());

		lat->clear();
		s->lat->init();
		for (Map *p = s->lat; p->hasNext(); )
			lat->add(p->next());

		col->clear();
		s->col->init();
		for (Map *p = s->col; p->hasNext(); )
			col->add(p->next());

	}

	virtual void apply(Event *e)
	{
		Patient *p = e->getPatient();
		if (p == 0)
			return;

		switch(e->getType())
		{
		case acquisition:
			sus->remove(p);
			if (nStates() == 2)
				col->add(p);
			else
				lat->add(p);
			break;

		case progression:
			lat->remove(p);
			col->add(p);
			break;

		case clearance:
			col->remove(p);
			sus->add(p);
			break;
			
		
		case insitu:
		case insitu0:
		case admission0:
		case admission:
			pat->add(p);
			sus->add(p);
			break;
			
		case insitu1:
		case admission1:
			pat->add(p);
			lat->add(p);
			break;

		case insitu2:
		case admission2:
			pat->add(p);
			col->add(p);
			break;

		case discharge:
			sus->remove(p);
			col->remove(p);
			lat->remove(p);
			pat->remove(p);
			break;

		default:
			break;
		}
	}

	virtual void unapply(Event *e)
	{
		Patient *p = e->getPatient();
		if (p == 0)
			return;

		switch(e->getType())
		{
		case acquisition:
			sus->add(p);
			if (nStates() == 2)
				col->remove(p);
			else
				lat->remove(p);
			break;

		case progression:
			lat->add(p);
			col->remove(p);
			break;

		case clearance:
			col->add(p);
			sus->remove(p);
			break;

		default:
			break;
		}
	}

	void write(ostream &os)
	{
		os << sus->size() << "+" << lat->size() << "+" << col->size() << "=";
		os << sus->size() + lat->size() + col->size();
		os << "=" << pat->size();
	}

};
