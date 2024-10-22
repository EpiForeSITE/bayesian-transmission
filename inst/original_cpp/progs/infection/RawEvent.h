
class RawEvent : public Event, public EventCoding
{
public:
	int pat;
	int type;
	int unit;

	RawEvent(double t, int p, int tp) : Event(t)
	{
		pat = p;
		type = tp;
		unit = 0;
	}

	RawEvent(int un, double t, int p, int tp) : Event(t)
	{
		pat = p;
		type = tp;
		unit = un;
	}

	int compare (Object *e)
	{
		RawEvent *x = dynamic_cast<RawEvent *>(e);
		if (x == 0)
			return Event::compare(e);
		
		if (x->pat < pat)
			return 1;
		if (pat < x->pat)
			return -1;

		if (x->time < time)
			return 1;
		if (time < x->time)
			return -1;

		if (x->unit < unit)
			return 1;
		if (unit < x->unit)
			return -1;

		if (code(x->type) < code(type))
			return 1;
		if (code(type) < code(x->type))
			return -1;

		return 0;
	}

	string className()
	{
		return "RawEvent";
	}

	void write(ostream &os)
	{
		os << unit << "\t" << time << "\t" << pat << "\t" << type ;
	}
};
