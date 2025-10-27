
class HistoryLink : public Object
{
private:

	// Pointers to patient's adjacent links.
	HistoryLink *pprev;
	HistoryLink *pnext;

	// Pointers to unit's adjacent links.
	HistoryLink *uprev;
	HistoryLink *unext;
	
	// Pointers to facility's adjacent links.
	HistoryLink *fprev;
	HistoryLink *fnext;
	
	// Pointers to whole system's adjacent links.
	HistoryLink *sprev;
	HistoryLink *snext;

	// The system's, facitlity's, unit's and patient's state after the event.
	LocationState *sstate;
	LocationState *fstate;
	LocationState *ustate;
	PatientState *pstate;

	// Pointers to alternative proposed history links.
	HistoryLink *hprev;
	HistoryLink *hnext;

	// The event occuring at this point.
	Event *e;

	// Indicator for adding event to SystemHistory links.
	int linked;

	// Indicator for hiding event when cross validating.
	int hidden;

public:

	HistoryLink(Event *event, int l = 1)
	{
		e = event;
	
		setStates(0,0,0,0);

		pprev = 0;
		pnext = 0;
		uprev = 0;
		unext = 0;
		fprev = 0;
		fnext = 0;
		sprev = 0;
		snext = 0;
		hprev = 0;
		hnext = 0;

		linked = l;
		hidden = 0;
	}

	HistoryLink(Event *event, LocationState *s, LocationState *f, LocationState *u, PatientState *p, int l = 1)
	{
		e = event;
	
		setStates(s,f,u,p);

		pprev = 0;
		pnext = 0;
		uprev = 0;
		unext = 0;
		fprev = 0;
		fnext = 0;
		sprev = 0;
		snext = 0;
		hprev = 0;
		hnext = 0;

		linked = l;
		hidden = 0;
	}

	~HistoryLink()
	{
		if (sstate != 0)
			delete sstate;
		if (fstate != 0)
			delete fstate;
		if (ustate != 0)
			delete ustate;
		if (pstate != 0)
			delete pstate;
	}

	void setStates(LocationState *s, LocationState *f, LocationState *u, PatientState *p)
	{
		sstate = s;
		fstate = f;
		ustate = u;
		pstate = p;
	}

	void setCopyApply()
	{
		if (pstate != 0)
		{
			if (pprev != 0)
				pstate->copy(pprev->pstate);
			pstate->apply(e);
		}

		if (ustate != 0)
		{
			if (uprev != 0)
			{
				ustate->copy(uprev->ustate);
			}
			ustate->apply(e);
		}

		if (fstate != 0)
		{
			if (fprev != 0)
				fstate->copy(fprev->fstate);
			fstate->apply(e);
		}

		if (sstate != 0)
		{
			if (sprev != 0)
				sstate->copy(sprev->sstate);
			sstate->apply(e);
		}
	}

	inline void insertAsap(HistoryLink *y)
	{
		HistoryLink *snxt = y;

		for ( ; snxt != 0 && snxt->getEvent()->getTime() < getEvent()->getTime(); snxt = snxt->snext);
		for ( ; snxt != 0 && snxt->sprev != 0 && snxt->sprev->getEvent()->getTime() >= getEvent()->getTime(); snxt = snxt->sprev);

		if (snxt == 0)
			cerr << "System run off \n";

		insertBeforeS(snxt);

		HistoryLink *xx = 0;

		for (xx = snxt ; xx != 0 && xx->getEvent()->getFacility() != getEvent()->getFacility(); xx = xx->snext);
		if (xx == 0)
			cerr << "Facility run off \n";
		insertBeforeF(xx);

		for (xx = snxt ; xx != 0 && xx->getEvent()->getUnit() != getEvent()->getUnit(); xx = xx->snext);
		if (xx == 0)
			cerr << "Unit run off \n";
		insertBeforeU(xx);

		for (xx = snxt ; xx != 0 && xx->getEvent()->getPatient() != getEvent()->getPatient(); xx = xx->snext);
		if (xx == 0)
			cerr << "Patient run off \n";
		insertBeforeP(xx);
	}

	inline void insertBeforeS(HistoryLink *x)
	{
		snext = x;
		sprev = x->sprev;
		x->sprev = this;
		if (sprev != 0)
			sprev->snext = this;
	}

	inline void insertBeforeF(HistoryLink *x)
	{
		fnext = x;
		fprev = x->fprev;
		x->fprev = this;
		if (fprev != 0)
			fprev->fnext = this;
	}

	inline void insertBeforeU(HistoryLink *x)
	{
		unext = x;
		uprev = x->uprev;
		x->uprev = this;
		if (uprev != 0)
			uprev->unext = this;
	}

	inline void insertBeforeP(HistoryLink *x)
	{
		pnext = x;
		pprev = x->pprev;
		x->pprev = this;
		if (pprev != 0)
			pprev->pnext = this;
	}

	inline void insertAfterP(HistoryLink *x)
	{
		pprev = x;
		pnext = x->pnext;
		x->pnext = this;
		if (pnext != 0)
			pnext->pprev = this;
	}

	inline void remove()
	{
		snext->sprev = sprev;
		sprev->snext = snext;
		fnext->fprev = fprev;
		fprev->fnext = fnext;
		unext->uprev = uprev;
		uprev->unext = unext;
		pnext->pprev = pprev;
		pprev->pnext = pnext;
	}

	inline void removeS()
	{
		snext->sprev = sprev;
		sprev->snext = snext;
	}

	inline void removeF()
	{
		fnext->fprev = fprev;
		fprev->fnext = fnext;
	}

	inline void removeU()
	{
		unext->uprev = uprev;
		uprev->unext = unext;
	}

	inline void removeP()
	{
		pnext->pprev = pprev;
		pprev->pnext = pnext;
	}

	void write(ostream &os, int opt)
	{
		//os << e;
		e->write(os,opt);
		os << "\t::\t";
	//	os << sstate;
	//	os << "\t::\t";
	//	os << fstate;
	//	os << "\t::\t";
		os << ustate;
		os << "\t::\t";
		os << pstate;
		
	}

	void write(ostream &os)
	{
		write(os,0);
	}

// Trivial accessors.

	inline void setLinked(int i) 
	{ 
		linked = i; 
	}

	inline int isLinked() 
	{ 
		return linked; 
	}

	inline void setHidden(int i) 
	{ 
		hidden = i; 
	}

	inline int isHidden() 
	{ 
		return hidden; 
	}

	inline HistoryLink *hPrev() 
	{ 
		return hprev; 
	} 

	inline HistoryLink *hNext() 
	{ 
		return hnext; 
	}

	inline void setHPrev(HistoryLink *l) 
	{ 
		hprev = l;
	}

	inline void setHNext(HistoryLink *l) 
	{ 
		hnext = l; 
	}

	inline Event *getEvent() 
	{ 
		return e; 
	} 

	inline void clearEvent()
	{
		e = 0;
	}

	inline HistoryLink *pPrev() 
	{ 
		return pprev; 
	} 

	inline HistoryLink *pNext() 
	{ 
		return pnext; 
	}

	inline HistoryLink *uPrev() 
	{ 
		return uprev; 
	} 

	inline HistoryLink *uNext() 
	{ 
		return unext; 
	}

	inline HistoryLink *fPrev() 
	{ 
		return fprev; 
	} 

	inline HistoryLink *fNext() 
	{ 
		return fnext; 
	}

	inline HistoryLink *sPrev() 
	{ 
		return sprev; 
	} 

	inline HistoryLink *sNext() 
	{ 
		return snext; 
	}

	inline PatientState *getPState() 
	{ 
		return pstate;
	}

	inline LocationState *getUState() 
	{ 
		return ustate; 
	}

	inline LocationState *getFState() 
	{ 
		return fstate; 
	}

	inline LocationState *getSState() 
	{ 
		return sstate; 
	}
};

