
class EventCoding 
{
public:
	// Make sure that this is larger than any of the event codes below.
	static const int maxeventcode = 100;

	enum EventCode
	{
		nullevent	 = -1,

		// Observed events.

		admission 	 = 0,
		negsurvtest 	 = 1,
		possurvtest 	 = 2,
		discharge 	 = 3,
		negclintest	 = 4,
		posclintest	 = 5,
		abxdose		= 9,
		abxon		= 10,
		abxoff		= 11,
		isolon		= 12,
		isoloff		= 13,
	
		postest 	= 7,
		negtest		= 8,

		// Inferred events.

		admission0	 = 14,
		admission1	 = 15,
		admission2	 = 16,

		insitu 		 = 6,
		insitu0		 = 17,
		insitu1		 = 18,
		insitu2		 = 19,

		acquisition	 = 31,
		progression 	 = 32,
		clearance	 = 33,

		// Marker events.
		start 		 = 21,
		marker 		 = 22,
		stop 		 = 23,

		error 		 = -999
	};

	static string eventString(int x)
	{
		return eventString((EventCode)x);
	}

	static string eventString(EventCode x)
	{
		switch(x)
		{
		case nullevent:		return "nullevent";
		case start: 		return "start";
		case admission: 	return "admission";
		case admission0: 	return "admission0";
		case admission1: 	return "admission1";
		case admission2: 	return "admission2";
		case negtest: 		return "negtest";
		case postest: 		return "postest";
		case negsurvtest: 	return "negsurvtest";
		case possurvtest: 	return "possurvtest";
		case negclintest: 	return "negclintest";
		case posclintest: 	return "posclintest";
		case discharge: 	return "discharge";
		case acquisition: 	return "acquisition";
		case clearance: 	return "clearance";
		case progression:	return "progression";
		case insitu: 		return "insitu";
		case insitu0:		return "insitu0";
		case insitu1:		return "insitu1";
		case insitu2:		return "insitu2";
		case marker: 		return "marker";
		case stop: 		return "stop";
		case abxdose: 		return "abxdose";
		case abxon: 		return "abxon";
		case abxoff: 		return "abxoff";
		case isolon:		return "isolon";
		case isoloff:		return "isoloff";
		case error: 		return "EVENT_TYPE_ERROR";
		//default: 		return "CAN'T REACH THIS";
		}
		return "CAN'T REACH THIS";
	}
};
