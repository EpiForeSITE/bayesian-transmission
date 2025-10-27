class UnitTrackingModel : public Model
{
protected:

	int nstates;
	Map *units;

public:

	UnitTrackingModel(int ns = 2)
	{
		nstates = ns;
		units = new Map();
	}

	virtual inline Map *getUnits()
	{
		units->init();
		return units;
	}

	virtual inline int getNStates()
	{
		return nstates;
	}

/*
	virtual HistoryLink *makeHistLink(Facility *f, Unit *u, double time, Patient *p, EventCode type, int linked)
	{
		units->put(u,0);

		return new HistoryLink
		(	
			new Event(f,u,time,p,type),
			makeSystemState(),	
			makeFacilityState(f),	
			makeUnitState(u),	
			makePatientState(p),
			linked	
		);
	}
*/

	virtual string header()
	{
		return "";
	}

	virtual void write (ostream &os)
	{
	}

	virtual int needEventType(EventCode e)
	{
		return true;
	}

	virtual LocationState *makeSystemState()
	{
		return 0;
	}

	virtual LocationState *makeFacilityState(Facility *f)
	{
		return 0;
	}

	virtual LocationState *makeUnitState(Unit *u)
	{
		return u == 0 ? 0 : new SetLocationState(u,nstates);
		//return u == 0 ? 0 : new SetLocationState(u,nstates);
	}

	virtual PatientState *makePatientState(Patient *p)
	{
		return p == 0 ? 0 : new PatientState(p,nstates);
	}

	virtual EpisodeHistory *makeEpisodeHistory(HistoryLink *a, HistoryLink *d)
	{
		return new UnitEpisodeHistory(a,d);
	}

// Dummy methods to satisfy Model requirements.

	virtual double logLikelihood(SystemHistory *h)
	{
		return 0;
	}

	virtual void forwardSimulate(SystemHistory *h, Random *r)
	{
	}
		
	virtual void initEpisodeHistory(EpisodeHistory *h, bool b)
	{
	}

	virtual void sampleEpisodes(SystemHistory *h, int i, Random *r)
	{
	}

	virtual void update(SystemHistory *h, Random *r, int i)
	{
	}
};
