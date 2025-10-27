
class RawEventList : public SortedList
{
public:

	static int maxline;

	RawEventList(istream &is, stringstream &err) : SortedList()
	{
		char *c = new char[maxline];

		for (int line=1; !is.eof(); line++)
		{
			is.getline(c,maxline);
			if (is.eof())
				break;

			int facility = 0;
			int unit = 0;
			double time = 0;
			int patient = 0;
			int type = 0;

			if (sscanf(c,"%d%d%lf%d%d",&facility,&unit,&time,&patient,&type) != 5)
			{
				err << "Line " << line << ": Format problem:\n";
				err << "\t" << c << "\n";
				continue;
			}

			append(new RawEvent(facility,unit,time,patient,type));
		}
		
		init();

		delete [] c;
	}
	
	~RawEventList()
	{
		for (init(); hasNext(); )
		{
			delete next();
		}

		clear();
	}

	double firstTime()
	{
		double x = 0;
		init();
		if (hasNext())
			x = (dynamic_cast<RawEvent *>(next()))->getTime();

		for (init(); hasNext(); )
		{
			double y = (dynamic_cast<RawEvent *>(next()))->getTime();
			if (y < x)
				x = y;
		}

		return x;
	}

	double lastTime()
	{
		double x = 0;
		init();
		if (hasNext())
			x = (dynamic_cast<RawEvent *>(next()))->getTime();


		for (init(); hasNext(); )
		{
			double y = (dynamic_cast<RawEvent *>(next()))->getTime();
			if (y > x)
				x = y;
		}

		return x;
	}

	void write(ostream &os)
	{
		int i = 0;
		for (init(); hasNext(); )
		{
			if (i++ != 0)
				os << "\n";
			os << next();
		}
	}
};

int RawEventList::maxline = 1000;
