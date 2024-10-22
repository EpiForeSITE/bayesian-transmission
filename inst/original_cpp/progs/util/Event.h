
class Event : public Object
{
protected:
	double time;

public:
	Event(double t) : Object()
	{
		time = t;
	}

	void setTime(double t)
	{
		time = t;
	}

	const double getTime() const
	{
		return time;
	}

	int compare(Object *e)
	{
		Event *x = dynamic_cast<Event *>(e);
		if (x == 0)
			return Object::compare(e);

		if (x->time < time)
			return 1;

		if (time < x->time)
			return -1;

		return 0;
	}

	string className()
	{
		return "Event";
	}

	void write(ostream &os)
	{
		Object::write(os);
		os << "(" << time << ")";
	}
};
