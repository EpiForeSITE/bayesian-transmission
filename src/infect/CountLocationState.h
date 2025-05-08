// infect/CountLocationState.h
#ifndef ALUN_INFECT_COUNTLOCATIONSTATE_H
#define ALUN_INFECT_COUNTLOCATIONSTATE_H

#include "LocationState.h"

class CountLocationState : public LocationState
{
private:

	int tot;
	int inf;
	int lat;

public:

	CountLocationState(Object *own, int nstates = 0);

	virtual void clear();
	virtual int getTotal() const;
	virtual int getColonized() const;
	virtual int getLatent() const;
	virtual int getSusceptible() const;

	virtual void copy(State *s);
	virtual void apply(Event *e);
	virtual void unapply(Event *e);
	virtual void write(ostream &os) const;
};
#endif // ALUN_INFECT_COUNTLOCATIONSTATE_H
