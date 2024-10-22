
enum EventCode
{
	start,
	admission,
	negtest,
	colonization,
	decolonization,
	postest,
	discharge,
	insitu,
	marker,
	stop,
	error
};

class EventCoding
{
public:
	static EventCode code(int x)
	{
		switch(x)
		{
		case -1: 
			return start;
		case 0: 
			return admission;
		case 1: 
			return negtest;
		case 2: 
			return postest;
		case 3: 
			return discharge;
		case 4: 
			return colonization;
		case 5:
			return decolonization;
		case 6: 
			return insitu;
		case 8:
			return marker;
		case 9: 
			return stop;
		default:
			return error;
		}
	}

	static int decode(EventCode x)
	{
		switch(x)
		{
		case start:
			return -1;
		case admission:
			return 0;
		case negtest:
			return 1;
		case postest:
			return 2;
		case discharge:
			return 3;
		case colonization:
			return 4;
		case decolonization:
			return 5;
		case insitu:
			return 6;
		case marker:
			return 8;
		case stop:
			return 9;
		case error:
		default:
			return -999;
		}
	}

	static string toString(EventCode x)
	{
		switch(x)
		{
		case start:
			return "start";
		case admission:
			return "admission";
		case negtest:
			return "negtest";
		case postest:
			return "postest";
		case discharge:
			return "discharge";
		case colonization:
			return "colonization";
		case decolonization:
			return "decolonization";
		case insitu:
			return "insitu";
		case marker:
			return "marker";
		case stop:
			return "stop";
		case error:
			return "EVENT_TYPE_ERROR";
		default:
			return "CAN'T REACH THIS";
		}
	}
};
