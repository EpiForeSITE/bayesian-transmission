

class EpisodeHistory : public Object, public EventCoding
{
protected:

	virtual void applyInitialEvent(Event *e) = 0;

	virtual void unapplyInitialEvent(Event *e) = 0;

	virtual void applyAndInsert(HistoryLink *l) = 0;

	virtual void removeAndUnapply(HistoryLink *l) = 0;

	HistoryLink *a;
	HistoryLink *d;

	double ta;
	double td;

	HistoryLink *h;
	HistoryLink *t;

	HistoryLink *pt;
	HistoryLink *ph;

public:

	EpisodeHistory(HistoryLink *aa, HistoryLink *dd)
	{
		h = 0;
		t = 0;

		ph = 0;
		pt = 0;
			
		a = aa;
		d = dd;

		ta = a->getEvent()->getTime();
		td = d->getEvent()->getTime();
	}

	~EpisodeHistory()
	{
		for (HistoryLink *l = h; l != 0; )
		{
			HistoryLink *ll = l->hNext();
			delete l->getEvent();
			if (!l->isLinked())
				delete l;
			l = ll;
		}
	}

	void removeEvents(List *list)
	{
		for (HistoryLink *l = h; l != 0; )
		{
			HistoryLink *ll = l->hNext();
			list->remove(l->getEvent());
			l = ll;
		}
	}

	EventCode eventPreAdmission()
	{
		EventCode c = nullevent;

		for (HistoryLink *l = ph; l != 0; l = l->hNext())
			if (!l->isLinked())
				c = l->getEvent()->getType();
		return c;
	}

	void proposeSwitch(HistoryLink *l)
	{
		if (ph == 0)
		{
			l->setHNext(0);
			l->setHPrev(0);
			ph = l;
			pt = l;
		}
		else
		{
			l->setHNext(0);
			l->setHPrev(pt);
			pt->setHNext(l);
			pt = l;
		}
	}

	HistoryLink *getProposalHead()
	{
		return ph;
	}

	int countProposedSwitches()
	{
		int x = 0;
		for (HistoryLink *l = ph; l != 0; l = l->hNext())
			x += l->isLinked();
		return x;
	}

	int countSwitches()
	{
		int x = 0;
		for (HistoryLink *l = h; l != 0; l = l->hNext())
			x += l->isLinked();
		return x;
	}

	int proposalDifferent()
	{
		HistoryLink *l = h;
		HistoryLink *pl = ph;

		while(l != 0 || pl != 0)
		{
			if (l == 0)
				return 1;
			if (pl == 0)
				return 1;

			if (l->getEvent()->getType() != pl->getEvent()->getType())
				return 1;

			if (l->getEvent()->getTime() != pl->getEvent()->getTime())
				return 1;
			
			l = l->hNext();
			pl = pl->hNext();
		}

		return 0;
	}

	void clearProposal()
	{
		for (HistoryLink *l = ph; l != 0; )
		{
			HistoryLink *ll = l->hNext();
			delete l->getEvent();
			delete l;
			l = ll;
		}
		ph = 0;
		pt = 0;
	}

	void installProposal()
	{
		HistoryLink *l = h;
		h = ph;
		ph = l;
		
		l = t;
		t = pt;
		pt = l;
	}

	void appendLink(HistoryLink *l)
	{
		unapply();
		installProposal();
		proposeSwitch(l);
		installProposal();
		apply();
	}

	void apply()
	{
		for (HistoryLink *l = h; l != 0; l = l->hNext())
		{
			if (l->isLinked())
				applyAndInsert(l);
			else
				applyInitialEvent(l->getEvent());
		}
	}

	void unapply()
	{
		for (HistoryLink *l = t; l != 0; l = l->hPrev())
		{
			if (l->isLinked())
				removeAndUnapply(l);
			else
				unapplyInitialEvent(l->getEvent());
		}
	}

	inline HistoryLink *admissionLink() 
	{ 
		return a; 
	}

	inline HistoryLink *dischargeLink() 
	{ 
		return d; 
	}

	inline double admissionTime() 
	{ 
		return ta; 
	}

	inline double dischargeTime() 
	{ 
		return td; 
	}

	void write(ostream &os)
	{
		for (HistoryLink *l = h; l != 0; l = l->hNext())
		{
			if (!l->isLinked())
				os << "\t-\t" << l << "\n";
			else
				os << "\t+\t" << l << "\n";
		}
		
		//for (HistoryLink *l = a; ; l = l->pNext())
		for (HistoryLink *l = a; ; l = l->uNext())
		{
			if (!l->isLinked())
				os << "**\t-\t" << l << "\n";
			else
				os << "**\t+\t" << l << "\n";

			if (l == d)
				break;
		}
	}
};
