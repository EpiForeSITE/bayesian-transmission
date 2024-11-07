// infect/Episode.h
#ifndef ALUN_INFECT_EPISODE_H
#define ALUN_INFECT_EPISODE_H

#include "../util/util.h"
#include "Event.h"

class Episode : public Object
{
private:

	Event *a;
	Event *d;
	SortedList *s;

public:
	Episode() : Object()
	{
		a = 0;
		d = 0;
		s = new SortedList();
	}

	~Episode()
	{
		delete s;
	}

	void write(ostream &os)
	{
		os << a << "\n";
		for (s->init(); s->hasNext(); )
			os << s->next() << "\n";
		os << d;
	}

	inline void setAdmission(Event *e)
	{
		a = e;
		s->prepend(e);
	}

	inline Event *getAdmission()
	{
		return a;
	}

	inline void setDischarge(Event *e)
	{
		d = e;
		s->append(e);
	}

	inline Event *getDischarge()
	{
		return d;
	}

	inline void addEvent(Event *e)
	{
		s->append(e);
	}

	inline SortedList *getEvents()
	{
		s->init();
		return s;
	}
};
#endif // ALUN_INFECT_EPISODE_H
