// infect/State.h
#ifndef ALUN_INFECT_STATE_H
#define ALUN_INFECT_STATE_H

#include "Event.h"

class State : public Object, public EventCoding
{
private:
	Object *owner;

protected:

        bool ownerWantsEvent(Event *e)
        {
                Unit *u = dynamic_cast<Unit *>(getOwner());
                if (u)
                        return u == e->getUnit();

                Facility *f = dynamic_cast<Facility *>(getOwner());
                if (f)
                        return f == e->getFacility();

                if (getOwner() == 0)
                        return 1;

                cerr << "Unknown owner type: " << getOwner() << "\n";
                return 0;
        }

public:

	virtual void copy(State *s) = 0;
	virtual void apply(Event *e) = 0;
	virtual void unapply(Event *e) = 0;

	State(Object *o)
	{
		owner = o;
	}

	virtual ~State()
	{
	}

	void write(ostream &os) const
	{
        	os << owner;
	}

	inline Object *getOwner() const
	{
		return owner;
	}
};

#endif // ALUN_INFECT_STATE_H
