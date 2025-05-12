// infect/LocationState.h
#ifndef ALUN_INFECT_LOCATIONSTATE_H
#define ALUN_INFECT_LOCATIONSTATE_H

#include "State.h"
#include "Event.h"
namespace infect
{

class LocationState : public State
{
private:

	int n;

public:

	LocationState(Object *own, int nstates = 0) : State(own), n(nstates){}

	inline int nStates() const {return n;}

	virtual void clear() = 0;
	virtual int getTotal() const = 0;
	virtual int getColonized() const = 0;
	virtual int getLatent() const = 0;
	virtual int getSusceptible() const = 0;
	virtual void copy(State *s) = 0;
	virtual void apply(Event *e) = 0;
	virtual void unapply(Event *e) = 0;


	virtual void write(ostream &os) const;
};

} // namespace infect
#endif // ALUN_INFECT_LOCATIONSTATE_H
