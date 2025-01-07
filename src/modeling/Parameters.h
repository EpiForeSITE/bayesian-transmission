#ifndef ALUN_MODELING_PARAMETERS_H
#define ALUN_MODELING_PARAMETERS_H

#include "../infect/infect.h"
#include <string>
#include <vector>

class Parameters : public Object, public infect::EventCoding, public infect::InfectionCoding
{
public:

	virtual double logProb(infect::HistoryLink *h) = 0;
	virtual double logProbGap(infect::HistoryLink *g, infect::HistoryLink *h) { return 0; }

	virtual void initCounts() = 0;
	virtual void count(infect::HistoryLink *h) = 0;
	virtual void countGap(infect::HistoryLink *g, infect::HistoryLink *h) { }

	virtual void update(Random *r, bool max) = 0;

	virtual string header() = 0;
	virtual std::vector<std::string> paramNames() const = 0;

	virtual int getNStates() const = 0;


	virtual void update(Random *r)
	{
		update(r,0);
	}

	virtual std::vector<double> getValues() const = 0;

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

	virtual inline int stateIndex(InfectionStatus s) const
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

	virtual inline int testResultIndex(EventCode e) const
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
#endif // ALUN_MODELING_PARAMETERS_H
