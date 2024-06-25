
class Event : public Object, public EventCoding
{
private:

	Facility *fac;
	Unit *unit;
	double time;
	Patient *pat;
	EventCode type;

public:

	Event(Facility *fa, Unit *un, double tm, Patient *pt, EventCode tp)
	{
		fac = fa;
		unit = un;
		time = tm;
		pat = pt;
		type = tp;
	}

	inline double getTime() 
	{ 
		return time; 
	}

	inline void setTime(double t)
	{
		time = t;
	}

	inline Facility *getFacility() 
	{ 
		return fac; 
	}

	inline Unit *getUnit() 
	{ 
		return unit; 
	}

	inline Patient *getPatient() 
	{ 
		return pat; 
	}

	inline EventCode getType() 
	{ 
		return type; 
	}

	inline void setType(EventCode c) 
	{ 
		type = c; 
	}

	inline bool isTest()
	{
		switch(type)
		{
		case postest:
		case negtest:
		case negsurvtest: 	
		case possurvtest:
		case negclintest:
		case posclintest:
			return 1;
		default:
			return 0;
		}
	}

	inline bool isPositiveTest()
	{
		switch(type)
		{
		case postest:
		case possurvtest:
		case posclintest:
			return 1;
		default:
			return 0;
		}
	}

	inline bool isClinicalTest()
	{
		switch(type)
		{
		case posclintest:
		case negclintest:
			return 1;
		default:
			return 0;
		}
	}

	inline bool isAdmission()
	{
		switch(type)
		{
		case admission:
		case admission0:
		case admission1:
		case admission2:
			return 1;
		default:
			return 0;
		}
	}
	
	inline bool isInsitu()
	{
		switch(type)
		{
		case insitu:
		case insitu0:
		case insitu1:
		case insitu2:
			return 1;
		default:
			return 0;
		}
	}

	inline bool isObservable()
	{
		switch(type)
		{
		case insitu:
		case insitu0:
		case insitu1:
		case insitu2:
		case admission:
		case admission0:
		case admission1:
		case admission2:
		case discharge:
		case negsurvtest: 	
		case possurvtest:
		case negclintest:
		case posclintest:
		case abxdose:
		case abxon:
		case abxoff:
			return 1;
		default:
			return 0;
		}
	}

	inline bool isCollonizationEvent()
	{
		switch(type)
		{
		case acquisition:
		case progression:
		case clearance:
			return 1;
		default:
			return 0;
		}
	}

	void write(ostream &os)
	{
		write(os,0);
	}

	void write(ostream &os, int opt)
	{
		int f = fac == 0 ? 0 : fac->getId();
		int u = unit == 0 ? 0 : unit->getId();
		int p = pat == 0 ? 0 : pat->getId();

		EventCode otype = type;

		switch(type)
		{
		case insitu:
			otype = admission;
			break;

		case insitu0:
			otype = ( opt ? admission0 : admission );
			break;

		case insitu1:
			otype = ( opt ? admission1 : admission );
			break;

		case insitu2:
			otype = ( opt ? admission2 : admission );
			break;

		case admission0:
		case admission1:
		case admission2:
			otype = ( opt ? type : admission );
			break;

		default:
			otype =  type;
			break;
		}

		os << fixed;

		os << (f == 0 ? 0 : f) << "\t";
		os << (u == 0 ? 0 : u) << "\t";
		os << time << "\t";
		os << (p == 0 ? 0 : p) << "\t";
		os << otype;

		os.unsetf(ios::fixed | ios::scientific);
	}
};
