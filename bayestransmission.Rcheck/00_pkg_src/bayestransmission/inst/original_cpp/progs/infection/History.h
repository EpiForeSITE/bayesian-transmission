
class History : public Object, public EventStack
{
private:
	EpisodeEvent *head;
	EpisodeEvent *tail;
	IntMap *map;

	void append(EpisodeEvent *e, int deltot, int delinf)
	{
		EpisodeEvent *x;

		for (x = tail->prev; e->time < x->time; x = x->prev)
		{
			x->tot += deltot;
			x->inf += delinf;
		}

		e->prev = x;
		e->next = e->prev->next;
		e->prev->next = e;
		e->next->prev = e;

		e->tot = e->prev->tot + deltot;
		e->inf = e->prev->inf + delinf;
	}

	void removeUpTo(EpisodeEvent *e, EpisodeEvent *ed)
	{
		int delinf = e->type == colonization ? 1 : -1;
		int status = e->type == colonization ? 1 : 0;

		for (EpisodeEvent *x = e->next; x != ed; x = x->next)
		{
			x->inf -= delinf;
			if (x->epis == e->epis)
				x->status = 1-status;
		}

		if (e->epis == ed->epis)
			ed->status = 1-status;

		e->next->prev = e->prev;
		e->prev->next = e->next;

		e->prev = 0;
		e->next = 0;
	}

	void insertBefore(EpisodeEvent *e, EpisodeEvent *ed)
	{
		int delinf = e->type == colonization ? 1 : -1;
		int status = e->type == colonization ? 1 : 0;

		EpisodeEvent *x;

		if (e->epis == ed->epis)
			ed->status = status;

		for (x = ed->prev; e->time < x->time; x = x->prev)
		{
			x->inf += delinf;
			if (x->epis == e->epis)
				x->status = status;
		}

		e->next = x->next;
		e->prev = e->next->prev;

		e->prev->next = e;
		e->next->prev = e;

		e->tot = e->prev->tot;
		e->inf = e->prev->inf + delinf;
		e->status = status;
	}

	void setImportation(Episode *ep, int imp, EpisodeEvent *ed)
	{
		int delinf = imp == 1 ? 1 : -1;
		int status = imp == 1 ? 1 : 0;
	
		if (ep == ed->epis)
			ed->status = status;

		for (EpisodeEvent *x = ed->prev; x->next != ep->a; x = x->prev)
		{
			x->inf += delinf;
			if (x->epis == ep)
				x->status = status;
		}
	}

public:
	History(RawEventList *rel, stringstream &err, bool init = 1) : EventStack()
	{
	// Get array of episodes.

		double timetol = 0.000001;

		map = rel->getPatientMap();

		double firstt = rel->firstTime();
		double lastt = rel->lastTime();

		Array *eps = rel->getEpisodes(this,map,firstt,lastt,err);

		lastt += timetol;
		
	// Make sorted list of events from episode events and extras
	// initializing counts of total patients.

		head = pop(firstt,start,0);
		tail = pop(lastt,stop,0);

		head->prev = 0;
		head->next = tail;
		tail->prev = head;
		tail->next = 0;

		for (int i=0; i<eps->size(); i++)
		{
			Episode *ep = dynamic_cast<Episode *>(eps->get(i));
			append(ep->a,1,0);

			SortedList *s = ep->s;
			for (s->init(); s->hasNext(); )
				append(dynamic_cast<EpisodeEvent *>(s->next()),0,0);

			append(ep->d,-1,0);
		}

	// Make initial history of colonizations.

		if (init)
		{
			for (int i=0; i<eps->size(); i++)
			{
				Episode *ep = dynamic_cast<Episode *>(eps->get(i));
	
				if (ep->hasPositiveTest())
				{
					if (ep->hasNegBeforePosTest())
					{
						double coltime = (ep->lastNegBeforePosTime() + ep->firstPositiveTime())/2;
			
						if (coltime > ep->a->time)
						{
							ep->h = pop(coltime,colonization,ep);
							ep->t = ep->h;
							insertBefore(ep->h,ep->d);
						}
						else
						{
							setImportation(ep,1,ep->d);
						}
					}
					else
					{
						setImportation(ep,1,ep->d);
					}
				}
			}
		}
	}

	~History() 
	{
		push(head);
	}

	EpisodeEvent *firstEvent()
	{
		return head;
	}

	EpisodeEvent *lastEvent()
	{
		return tail;
	}

	void addExtraEvent(EpisodeEvent *e)
	{
		append(e,0,0);
	}

	void switchHistory(Episode *ep, int *imp, EpisodeEvent **h, EpisodeEvent **t)
	{
		EpisodeEvent *hh = 0;
		EpisodeEvent *tt = 0;
		EpisodeEvent *ed = upperBound(ep,*imp,*h,*t);

		for (EpisodeEvent *x = ep->t; x != 0 ; )
		{
			EpisodeEvent *y = x;

			x = x->prev;

			if (y->epis == ep && (y->type == colonization || y->type == decolonization))
			{
				removeUpTo(y,ed);

				if (hh == 0)
				{
					hh = y;
					tt = y;
				}
				else
				{
					hh->prev = y;
					y->next = hh;
					hh = y;
				}

			}

			if (y == ep->h)
			{
				break;
			}
		}

		ep->h = *h;
		ep->t = *t;
		*h = hh;
		*t = tt;

		if (*imp != ep->a->status)
		{
			setImportation(ep,*imp,ed);
			(*imp) = 1-(*imp);
		}

		for (EpisodeEvent *x = ep->h; x != 0; )
		{
			EpisodeEvent *y = x;
			x = x->next;
			insertBefore(y,ed);
			if (y == ep->t)
				break;
		}
	}

	EpisodeEvent *lowerBound(Episode *ep, int imp, EpisodeEvent *h, EpisodeEvent *t)
	{
		if (imp != ep->a->status)
			return ep->a->prev;

		EpisodeEvent *hh = ep->d->prev;
		double lt = ep->d->time;

		if (ep->h != 0)
		{
			hh = ep->h; 
			lt = ep->h->time;
		}

		if (h != 0 && h->time < lt)
			lt = h->time;

		EpisodeEvent *x = hh;

		while (lt <= x->time && x->next != ep->a)
			x = x->prev;

		return x;
	}
	
	EpisodeEvent *upperBound(Episode *ep, int imp, EpisodeEvent *h, EpisodeEvent *t)
	{
		if (ep->t == 0 || t == 0)
			return ep->d;

		if (ep->t->type != t->type)
			return ep->d;

		double ht = ep->t->time > t->time ? ep->t->time : t->time;

		for (EpisodeEvent *x = ep->t->next; x != ep->d; x = x->next)
			if (x->time >= ht)
				return x;

		return ep->d;
	}
	
// Checking methods.

	EpisodeEvent *clone()
	{
		EpisodeEvent *h = 0;
		EpisodeEvent *t = 0;

		for (EpisodeEvent *e = head; e != 0; e = e->next)
		{
			EpisodeEvent *c = pop(e);
			c->tot = e->tot;
			c->status = e->status;
			c->inf = e->inf;

			if (h == 0)
			{
				h = t = c;
			}
			else
			{
				t->next = c;
				c->prev = t;
				t = c;
			}
		}
	
		return h;
	}

	int equals(EpisodeEvent *h)
	{
		double tol = 0.000001;
		EpisodeEvent *x = head;
		EpisodeEvent *y = h;

		while (true)
		{
			if (x == 0 && y == 0)
				return 1;

			if (x == 0 || y == 0)
				return 0;

			if (x->inf != y->inf)
				return 0;
			if (x->tot != y->tot)
				return 0;

			double t = x->time - y->time;
			if (abs(t) > tol)
				return 0;

			// Might not worry about the following if 
			// order can vary for simultaneous events.

			if (x->status != y->status)
				return 0;
			if (x->type != y->type)
				return 0;
			if (x->epis != y->epis)
				return 0;

			x = x->next;
			y = y->next;
		}

		return 1;
	}

	int check()
	{
		int tot = 0;
		int inf = 0;

		for (EpisodeEvent *e = head; e != 0; e = e->next)
		{
			switch(e->type)
			{
			case admission:
			case insitu:
				tot += 1;
				inf += e->status;
				break;
			case discharge:
				tot -= 1;
				inf -= e->status;
				break;
			case colonization:
				inf += 1;
				break;
			case decolonization:
				inf -= 1;
				break;
			default:
				break;
			}
			
			if (inf > tot || e->inf != inf || e->tot != tot)
				return 0;
		}

		return 1;
	}

// Class handling stuff.

	string className()
	{
		return "History";
	}

	void write(ostream &os)
	{
		int i = 0;
		for (EpisodeEvent *e = firstEvent(); e != 0; e = e->next)
			os << i++ << "\t" << e << "\n";
	}
};
