// infect/RawEvent.h
#ifndef ALUN_INFECT_RAWEVENT_H
#define ALUN_INFECT_RAWEVENT_H

#include "../util/util.h"

class RawEvent : public Object
{
private:

	double time;
	int facility;
	int unit;
	int pat;
	int type;

public:
	RawEvent(int f, int u, double t, int p, int tp)
	{
		facility = f;
		unit = u;
		time = t;
		pat = p;
		type = tp;
	}

	// RawEvents are sorted first by patient then by time
	// then by unit. This ordering is used by EventData
	// to create the checked lists of Events and Episodes.
	int compare (Object const * const e) const
	{
		RawEvent const * const x = dynamic_cast<RawEvent const * const>(e);

		if (x->pat < pat)
			return 1;
		if (pat < x->pat)
			return -1;

		if (x->time < time)
			return 1;
		if (time < x->time)
			return -1;

		if (x->facility < facility)
			return 1;
		if (facility < x->facility)
			return -1;

		if (x->unit < unit)
			return 1;
		if (unit < x->unit)
			return -1;

		return 0;
	}

	void write(ostream &os) const
	{
		os << facility << "\t";
		os << unit << "\t";
		os << time << "\t";
		os << pat << "\t";
		os << type;
	}

	inline double getTime() const
	{
		return time;
	}

	inline int getFacilityId() const
	{
		return facility;
	}

	inline int getUnitId() const
	{
		return unit;
	}

	inline int getPatientId() const
	{
		return pat;
	}

	inline int getTypeId() const
	{
		return type;
	}
};

#endif // ALUN_INFECT_RAWEVENT_H
