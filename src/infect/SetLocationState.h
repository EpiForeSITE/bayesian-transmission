// infect/SetLocationState.h
#ifndef ALUN_INFECT_SETLOCATIONSTATE_H
#define ALUN_INFECT_SETLOCATIONSTATE_H

#include "LocationState.h"
#include "InfectionCoding.h"

namespace infect
{
class SetLocationState : public LocationState, public InfectionCoding
{
protected:

	Map *pat;
	Map *sus;
	Map *lat;
	Map *col;

	int interSize(Map *a, Map *b);
	int interSize(Map *a, Map *b, int g);
	int subsetSize(Map *a, int g) const;

public:

	SetLocationState(Object *own, int ns = 0);

	~SetLocationState();

	inline void clear() override
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

	virtual int getTotal() const;
	virtual int getTotal(int g) const;
	virtual int getSusceptible() const;
	virtual int getSusceptible(int g) const;
	virtual int getLatent() const;
	virtual int getLatent(int g) const;
	virtual int getColonized() const;
	virtual int getColonized(int g) const;
	virtual void copy(State *ss);
	virtual void apply(Event *e);
	virtual void unapply(Event *e);
	void write(ostream &os) const override;

};
} // namespace infect
#endif // ALUN_INFECT_SETLOCATIONSTATE_H
