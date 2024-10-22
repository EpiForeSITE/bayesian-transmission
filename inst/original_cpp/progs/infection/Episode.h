
class Episode : public Object
{
friend class RawEventList;
friend class Patient;
friend class History;
friend class EpisodeSampler;
friend class Likelihood;

private:
	EpisodeEvent *a;
	EpisodeEvent *d;
	SortedList *s;
	int u;

	EpisodeEvent *h;
	EpisodeEvent *t;

	Episode *next;
	Episode *prev;

	Patient *p;

public:

	Episode(int unit, Patient *pat) : Object()
	{
		u = unit;
		p = pat;
		a = 0;
		d = 0;
		s = new SortedList();

		h = 0;
		t = 0;

		next = 0;
		prev = 0;
	}

	~Episode()
	{
		delete s;
	}

	Episode *getNext()
	{
		return next;
	}

	Episode *getPrev()
	{
		return prev;
	}

	int getUnit()
	{
		return u;
	}

	Patient *getPatient()
	{	
		return p;
	}

	EpisodeEvent *getAdmission()
	{
		return a;
	}

	EpisodeEvent *getDischarge()
	{
		return d;
	}

	SortedList *getSwabs()
	{
		return s;
	}

	int hasPositiveTest()
	{
		for (s->init(); s->hasNext(); )
		{
			EpisodeEvent *e = dynamic_cast<EpisodeEvent *>(s->next());
			if (e->type == postest)
				return 1;
		}
		return 0;
	}

	double firstPositiveTime()
	{
		double x = d->time;

		for (s->init(); s->hasNext(); )
		{
			EpisodeEvent *e = dynamic_cast<EpisodeEvent *>(s->next());
			if (e->type == postest)
			{
				double y = e->time;
				if (y < x)
					x = y;
			}
		}

		return x;
	}

	int hasNegBeforePosTest()
	{
		double firstpos = firstPositiveTime();
		for (s->init(); s->hasNext(); )
		{
			EpisodeEvent *e = dynamic_cast<EpisodeEvent *>(s->next());
			if (e->type == negtest && e->time < firstpos)
				return 1;
		}
		return 0;
	}

	double lastNegBeforePosTime()
	{
		double x = a->time;
		double firstpos = firstPositiveTime();
		
		for (s->init(); s->hasNext(); )
		{
			EpisodeEvent *e = dynamic_cast<EpisodeEvent *>(s->next());
		
			if (e->type == negtest && e->time < firstpos)
			{
				double y = e->time;
				if (y > x)
					x = y;
			}
		}

		return x;
	}

	void setAdmission(EpisodeEvent *e)
	{
		if (e->type == admission || e->type == insitu)
		{
			a = e;
			e->epis = this;
		}
	}

	void setDischarge(EpisodeEvent *e)
	{
		if (e->type == discharge)
		{
			d = e;
			e->epis = this;
		}
	}

	void addSwab(EpisodeEvent *e)
	{
		if (e->type == negtest || e->type == postest)
		{
			s->append(e);
			e->epis = this;
		}
	}

	string className()
	{
		return "Episode";
	}

	void write(ostream &os)
	{
	//	Object::write(os);
		int pat = 0;
		if (p)
			pat = p->id();

		os << pat << "(" << a->time << "||";
		for (s->init(); s->hasNext(); )
			os << (dynamic_cast<EpisodeEvent *>(s->next())->time) << ",";
		os << "||" << d->time << ")";
	}

	void checkout()
	{
		int errs = 0;

		//if (c->time < a->time)
		//	errs = 1;
		//if (c->time > d->time)
		//	errs = 2;

		for (s->init(); s->hasNext(); )
		{
			EpisodeEvent *e = dynamic_cast<EpisodeEvent *>(s->next());
			if (e->time < a->time)
				errs = 3;
			if (e->time > d->time)
				errs = 4;
		}

		//if (c->time < lastNegBeforePosTime())
		//	errs = 5;
			
		//if (c->time > firstPositiveTime())
		//	errs = 6;

		if (errs > 0)
		{
			cerr << errs << "   ERROR \n";
			write(cerr);
			cerr << "\n";
		}
	}
};

void EpisodeEvent::write(ostream &os)
{
	double ttt = time;
	int pat = -1;
	int unit = 0;
	if (epis != 0)
	{
		pat = epis->getPatient()->id();
		unit = epis->getUnit();
	}

	os << unit << "\t" << ttt << "\t" << pat << "\t" << decode(type) << "\t tot = " << tot << "\t inf = " << inf << "\t status = " << status;
}

void Patient::write(ostream &os)
{
	//Object::write(os);
	os << " ";
	//for (Episode *e = h; e != 0; e = e->next)
	//	os << "\t" << e;
	os << index;
}






