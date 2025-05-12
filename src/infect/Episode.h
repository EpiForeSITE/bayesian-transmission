// infect/Episode.h
#ifndef ALUN_INFECT_EPISODE_H
#define ALUN_INFECT_EPISODE_H

#include "../util/util.h"
#include "Event.h"

namespace infect
{
class Episode : public Object
{
private:

	Event *a;
	Event *d;
	SortedList *s;

public:
	Episode();
	~Episode()
    {
        delete s;
    }

	void write(ostream &os) const;

	inline void setAdmission(Event *e)
	{
		a = e;
		s->prepend(e);
	}

	inline Event *getAdmission() const
	{
		return a;
	}

	inline void setDischarge(Event *e)
	{
		d = e;
		s->append(e);
	}

	inline Event *getDischarge() const
	{
		return d;
	}

	inline void addEvent(Event *e)
	{
		s->append(e);
	}

	inline SortedList* getEvents()
	{
		s->init();
		return s;
	}
};
} // namespace infect
#endif // ALUN_INFECT_EPISODE_H
