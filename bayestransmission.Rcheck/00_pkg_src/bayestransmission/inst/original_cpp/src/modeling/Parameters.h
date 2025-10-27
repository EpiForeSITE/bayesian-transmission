
class Parameters : public Object, public EventCoding, public InfectionCoding
{
public:

	virtual double logProb(HistoryLink *h) = 0;
	virtual double logProbGap(HistoryLink *g, HistoryLink *h) { return 0; }

	virtual void initCounts() = 0;
	virtual void count(HistoryLink *h) = 0;
	virtual void countGap(HistoryLink *g, HistoryLink *h) { }

	virtual void update(Random *r, int max) = 0;

	virtual string header() = 0;

	virtual int getNStates() = 0;


	virtual void update(Random *r)
	{
		update(r,0);
	}

	virtual inline int eventIndex(EventCode e)
	{
		switch(e)
		{
		case acquisition:
			return 0;
		case progression:
			return 1;
		case clearance:
			return 2;
		default:
			return -1;
		}
	}

	virtual inline int stateIndex(InfectionStatus s)
	{
		switch(s)
		{
		case uncolonized:
			return 0;
		case latent:
			return 1;
		case colonized:
			return 2;
		default:
			return -1;
		}
	}

	virtual inline int testResultIndex(EventCode e)
	{
		switch(e)
		{
		case negtest:
		case negsurvtest:
		case negclintest:
			return 0;

		case postest:
		case possurvtest:
		case posclintest:
			return 1;

		default:
			return -1;
		}
	}
};
