
class AbxCoding
{
public:

	static Map *sysabx;
	static Map *syseverabx;

/*
	enum AbxStatus
	{
		nullstatus	= 0,
		offabx	 	= 1,
		onabx		= 2
	};

	static string abxCodeString(AbxStatus x)
	{
		switch(x)
		{
		case nullstatus:	return "null abx status";
		case offabx:		return "offabx";
		case onabx:		return "onabx";
		default: 		return "CAN'T REACH THIS";
		}
	}
*/
};

Map *AbxCoding::sysabx = new Map();
Map *AbxCoding::syseverabx = new Map();
