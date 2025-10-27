
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
	int compare (Object *e)
	{
		RawEvent *x = dynamic_cast<RawEvent *>(e);
		
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

	void write(ostream &os)
	{
		os << facility << "\t";
		os << unit << "\t";
		os << time << "\t";
		os << pat << "\t";
		os << type;
	}

	inline double getTime() 
	{ 
		return time; 
	} 

	inline int getFacilityId() 
	{ 
		return facility; 
	} 
	
	inline int getUnitId() 
	{ 
		return unit; 
	} 
	
	inline int getPatientId() 
	{ 
		return pat; 
	} 
	
	inline int getTypeId() 
	{ 
		return type; 
	} 
};
