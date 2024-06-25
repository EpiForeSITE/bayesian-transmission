
/**
	A RawEventList is a structure to accumulate events describing the
	history of a hospital unit. Apart from accumulating and sorting
	the events, the main point is to interperet these as a coherent
	list of patients and coherent sequence of patient episodes. 
	
	The getPatientMap method returns a map of integers to the patient
	structure indexed by that integer id.

	The getEpisodes methods takes the list of raw events and compiles 
	them into an array of Episodes checking that each is complete.
	Any errors have default fixes descriptions of which are written
	to a provided error stream.
*/

class RawEventList : public SortedList, public EventCoding
{
private:
	Patient *getpat(int i, IntMap *map)
	{
		Object *p = map->get(i);

		if (p == 0)
		{
			p = new Patient(i);
			map->put(i,p);
		}
		
		return dynamic_cast<Patient *>(p);
	}

	double minoutstay;

	static const double bigtime;

public:

	RawEventList() : SortedList()
	{
		minoutstay = 1;
	}

	static RawEventList *read(istream &is, stringstream &err)
	{
		RawEventList *l = new RawEventList();
		
		int maxline = 1000;
		char *c = new char[maxline];

		int unit = 0;

		for (int line=1; !is.eof(); line++)
		{
			is.getline(c,maxline);
			if (is.eof())
				break;

			double time = 0;
			int patient = 0;
			int type = 0;

			if (sscanf(c,"%lf%d%d",&time,&patient,&type) != 3)
			{
				break;
		//		err << "Line " << line << ": Format problem";
		//		err << "::\t" << c << "\n";
		//		continue;
			}

			double tt = time;
			l->add(unit,tt,patient,type);
		}
		
		return l;
	}

	static RawEventList **read(istream &is, int maxunit, stringstream &err)
	{
		RawEventList **l = new RawEventList*[maxunit];
		
		int maxline = 1000;
		char *c = new char[maxline];

		for (int line=1; !is.eof(); line++)
		{
			is.getline(c,maxline);
			if (is.eof())
				break;

			int unit = 0;
			double time = 0;
			int patient = 0;
			int type = 0;

			if (sscanf(c,"%d%lf%d%d",&unit,&time,&patient,&type) != 4)
			{
				err << "Line " << line << ": Format problem";
				err << "::\t" << c << "\n";
				continue;
			}

			if (unit >= maxunit)
			{
				err << "Line " << line << ": Unit id " << unit << " out of range";
				err << "::\t" << c << "\n";
				continue;
			}

			if (l[unit] == 0)
				l[unit] = new RawEventList();
		
			l[unit]->add(unit,time,patient,type);
		}
		
		return l;
	}

	void add(double t, int p, int k)
	{
		append(new RawEvent(t,p,k));
	}
	
	void add(int u, double t, int p, int k)
	{
		append(new RawEvent(u,t,p,k));
	}
	
	double firstTime()
	{
		double x = bigtime; 

		for (init(); hasNext(); )
		{
			double y = (dynamic_cast<RawEvent *>(next()))->getTime();
			if (x > y)
				x = y;
		}
		
		return x;
	}

	double lastTime()
	{
		double x = -bigtime;

		for (init(); hasNext(); )
		{
			double y = (dynamic_cast<RawEvent *>(next()))->getTime();
			if (x < y)
				x = y;
		}
		
		return x;
	}
	
/**
	Returns a map of integers to patient structures.
*/
	IntMap *getPatientMap()
	{
		IntMap *map = new IntMap();

		for (init(); hasNext(); )
		{
			int p = (dynamic_cast<RawEvent *>(next()))->pat;
			if (!map->got(p))
				map->put(p, new Patient(p));
		}

		return map;
	}

/**
	Returns an array of complete patient episodes. A patient may be 
	associated with multiple episodes.
	When admission, colonization or discharge events are missing, defaults,
	made under certain assumptions are inserted and error message writen to the 
	given stream.
*/

	Array *getEpisodes(EventStack *h, stringstream &err)
	{
		return getEpisodes(h, new IntMap(), firstTime(), lastTime(), err);
	}

	Array *getEpisodes(EventStack *h, IntMap *map, stringstream &err)
	{
		return getEpisodes(h, map, firstTime(), lastTime(), err);
	}

	Array *getEpisodes(EventStack *h, double start, double stop, stringstream &err)
	{
		return getEpisodes(h,new IntMap(), start, stop, err);
	}

	Array *getEpisodes(EventStack *h, IntMap *map, double start, double stop, stringstream &err)
	{
		// This is a map of patients to episodes that tracks any open event 
		// associated with the patient.

		Map *open = new Map();

		// This is a map of patients to episodes that tracks the last closed 
		// event associated with the patient.

		Map *closed = new Map();

		// This is the set of episodes that have been completely dealt with.
		// The set is implemeted as a map with episodes as keys, and null values.
		// Map is also a list that iterates through its elements in the order that
		// they were added.

		Map *done = new Map();

		for (init(); hasNext(); )
		{
			// Take the next event in the list.
			RawEvent *e = dynamic_cast<RawEvent *>(next());

			// Find the patient structure associated with the integer id in the 
			// provided id to patient structure map.
			Patient *p = getpat(e->pat,map);
			int unit = e->unit;

			// Find any open or closed episodes associated with this patient.

			Episode *s = dynamic_cast<Episode *>(open->get(p));
			Episode *t = dynamic_cast<Episode *>(closed->get(p));

			switch(code(e->type))
			{
			case admission:
				if (s != 0)
				{
					// There is already an open event associated with this patient.
					// This admission is ignored.

					err << "Adding admission event " << e << " for patient " << p << ".\n";
					err << "Error: The patient is presently admitted.\n";
					err << "Fix: Ignoring the current admission.\n";
					err << "\n";
					
					continue;
				}
				
				// Create a new episode with this admission, and add it to the open 
				// episodes set.

				s = new Episode(unit,p);
				s->setAdmission(h->pop(e->getTime(),admission,s));
				open->put(p,s);

				break;

			case discharge:
				if (s == 0)
				{
					// There is no open episode for this patient to be discharged from,
					// so the discharge is ignored.

					err << "Adding discharge event " << e << " for patient " << p << ".\n";
					err << "Error: The patient is presently not admitted.\n";
					err << "Fix: Ingoring the current discharge.\n";
					err << "\n";
				
					continue;
				}

				// Make a discharge event for the current open episode for this patient.
				// Move any previous closed episode to the done set, and add this episode to
				// the closed set.

				s->setDischarge(h->pop(e->getTime(),discharge,s));
				if (t != 0)
					done->add(t);
				closed->put(p,s);
				open->remove(p);

				break;

			case postest:
			case negtest:
				if (s == 0)
				{
					// There is no currently open episode for this patient, so create one
					// with default admission time equal to the later of the start time and 
					// the discharge time of any previous episode.

					double newt = ( t == 0 ? start : t->getDischarge()->getTime() );
					s = new Episode(unit,p);
					s->setAdmission(h->pop(newt,admission,s));
					open->put(p,s);
						
					err << "Adding ";
					err << (code(e->type) == postest ? "positive" : "negative");
					err << " test event " << e << " for patient " << p << ".\n";
					err << "Error: The patient is presently not admitted.\n";
					err << "Fix: Creating a new episode with admission time set to";
					err << (newt == start ? " start of study" : " time of previous discharge") << ".\n";
					err << "\n";
				}

				// Add the test event to the currently open episode for this patient.

				//s->addSwab(h->pop(e->getTime(),(e->type == postest ? postest : negtest),s));
				s->addSwab(h->pop(e->getTime(),code(e->type),s));

				break;

			case colonization:
				err << "Adding colonization event " << e << " for patient " << p << ".\n";
				err << "Error: Observed colonization events are not allowed.\n";
				err << "Fix: Ignoring event.\n";

				break;
			default:
				break;
			}
		}

		// Move each closed episode to the done set.

		for (closed->init(); closed->hasNext(); )
		{
			Patient *p = dynamic_cast<Patient *>(closed->next());
			Episode *e = dynamic_cast<Episode *>(closed->get(p));
			done->add(e);
			closed->remove(p);
		}

		// Close each open episode and add to the done set.

		for (open->init(); open->hasNext(); )
		{
			// Any episode that is still open at this stage is missing a discharge
			// event. Create one with default time the end of the study and move
			// the episode to the done set.

			Patient *p = dynamic_cast<Patient *>(open->next());
			Episode *e = dynamic_cast<Episode *>(open->get(p));
			e->setDischarge(h->pop(stop,discharge,e));
			done->add(e);
			open->remove(p);

			err << "Closing episode for patient " << p << "\n";
			err << "Error: Discharge event is missing.\n";
			err << "Fix: Adding discharge event at time equal to end of study period.\n";
			err << "\n";
		}

		// At this stage all required episodes should be in the done set.
		// Make patient episode list and merge episodes that are too close together.

		Map *merged = new Map();

		for (done->init(); done->hasNext(); )
		{
			// Take each episode and add it to the patient's list.
			// Merge episodes that are too close together.

			Episode *e = dynamic_cast<Episode *>(done->next());
			Patient *p = e->getPatient();

			if (p->h == 0)
			{
				p->h = e;
				p->t = e;
			}
			else
			{
				if (e->a->getTime() > p->t->d->getTime() + minoutstay)
				{
					p->t->next = e;
					e->prev = p->t;
					p->t = e;
				}
				else
				{
					h->push(p->t->d);
					h->push(e->a);

					p->t->setDischarge(e->d);
			
					List *l = e->getSwabs();
					for (l->init(); l->hasNext(); )
						p->t->addSwab(dynamic_cast<EpisodeEvent *>(l->next()));
	
					merged->add(e);
				}
			}
		}

		// Remove merged episodes from the done set.

		for (merged->init(); merged->hasNext(); )
			done->remove(merged->next());


		// If admission times are at the start of the study, change
		// change them to have insitu status.

		for (done->init(); done->hasNext(); )
		{
			EpisodeEvent *a = (dynamic_cast<Episode *>(done->next()))->getAdmission();
			if (a->getTime() <= start)
				a->setType(insitu);
		}


		// At this stage all required episodes should be in the done set.
		// Put the episodes in the output array.

		Array *result = new Array(done->size());
		int i = 0;
		for (done->init(); done->hasNext(); )
		{
			result->put(i++,done->next());
		}

		return result;
	}

	string className()
	{
		return "RawEventList";
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

const double RawEventList::bigtime = std::numeric_limits<double>::max()/10.0;
