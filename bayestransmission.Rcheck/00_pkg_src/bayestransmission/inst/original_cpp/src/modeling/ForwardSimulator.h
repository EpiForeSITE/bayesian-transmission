
class ForwardSimulator : public Object, EventCoding, InfectionCoding
{
public:

	static void forwardSimulate(UnitLinkedModel *mod, SystemHistory *hist, Random *rand)
	{
		if (!mod->isForwardEnabled())
		{
			cerr << "Model is not forward enabled. Cannot simulate.\n";
			return;
		}

		Map *ephist = new Map();

		for (Map *x = hist->getAdmissions(); x->hasNext(); )
		{
			Episode *e = (Episode *) x->next();
			HistoryLink *a = (HistoryLink *) x->get(e);
			ephist->put(a,hist->getEpisodes()->get(e));
		}

		Map *lastlink = new Map();

		for (HistoryLink *l = hist->getSystemHead(); l != 0; )
		{
			if (l->getEvent()->getUnit() != 0)
				lastlink->put(l->getEvent()->getUnit(),l);
			if (l->getEvent()->getPatient() != 0)
				lastlink->put(l->getEvent()->getPatient(),l);
	
			switch(l->getEvent()->getType())
			{
			case insitu:
			case insitu0:
			case insitu1:
			case insitu2:
			case admission:
			case admission0:
			case admission1:
			case admission2:
				randImportState(mod,l,getEpisodeHistory(ephist,l),rand);
				break;

			case postest:
			case negtest:
			case possurvtest:
			case negsurvtest:
			case posclintest:
			case negclintest:
				randTestResult(mod,l,rand);
				break;
	
			default:
				break;
			}

			if (l->sNext() == 0)
				break;

			HistoryLink *phl = 0;
			double time = l->sNext()->getEvent()->getTime();

			for (Map *mp = ((SetLocationState *)l->getSState())->getPatients(); mp->hasNext(); )
			{
				HistoryLink *pl = (HistoryLink *) lastlink->get(mp->next());
				HistoryLink *ul = (HistoryLink *) lastlink->get(pl->getEvent()->getUnit());

				double nexttime = randTimeToEvent(mod,l->getEvent()->getTime(),pl,ul,rand);

				if (nexttime < time)
				{
					time = nexttime;
					phl = pl;
				}
			}

			if (time < l->sNext()->getEvent()->getTime())
			{
				EventCode type = nullevent;

				switch(phl->getPState()->infectionStatus())
				{
				case uncolonized:
					type = acquisition;
					break;

				case latent:
					type = progression;
					break;

				case colonized:
					type = clearance;
					break;

				default:
					break;
				}

				HistoryLink *hl = mod->makeHistLink
				(
					phl->getEvent()->getFacility(),
					phl->getEvent()->getUnit(),
					time,
					phl->getEvent()->getPatient(),
					type,
					1
				);

				getEpisodeHistory(ephist,phl)->appendLink(hl);
				l = hl;
			}
			else
			{
				l = l->sNext();
			}
		}

		delete lastlink;
		delete ephist;
	}

protected:

	static EpisodeHistory *getEpisodeHistory(Map *map, HistoryLink *h)
	{
		for (HistoryLink *l = h; l != 0; l = l->pPrev())
		{
			switch(l->getEvent()->getType())
			{
			case insitu:
			case insitu0:
			case insitu1:
			case insitu2:
			case admission:
			case admission0:
			case admission1:
			case admission2:
				return (EpisodeHistory *) map->get(l);

			default:
				break;
			}
		}
		return 0;
	}

	static void randImportState(UnitLinkedModel *mod, HistoryLink *h, EpisodeHistory *eh, Random *rand)
	{
		Facility *f = h->getEvent()->getFacility();
		Unit *u = h->getEvent()->getUnit();
		Patient *p = h->getEvent()->getPatient();
		double atime = h->getEvent()->getTime();
		
		InfectionStatus prev = nullstatus;
		double time = 0;

		if (h->pPrev() != 0)
		{
			prev = h->pPrev()->getPState()->infectionStatus();
			time = atime - h->pPrev()->getEvent()->getTime();
		}

		if (h->getEvent()->isInsitu())
			h->getEvent()->setType(insitu0);
		if (h->getEvent()->isAdmission())
			h->getEvent()->setType(admission0);
	
		double U = rand->runif();

		if (mod->getNStates() == 2)
		{
			if (U < mod->getOutColParams()->transitionProb(prev,colonized,time))
			{
				eh->appendLink(mod->makeHistLink(f,u,atime,p,acquisition,0));
				if (h->getEvent()->isInsitu())
					h->getEvent()->setType(insitu2);
				if (h->getEvent()->isAdmission())
					h->getEvent()->setType(admission2);
			}
		}

		if (mod->getNStates() == 3)
		{
			if (U < mod->getOutColParams()->transitionProb(prev,latent,time))
			{
				eh->appendLink(mod->makeHistLink(f,u,atime,p,acquisition,0));
				if (h->getEvent()->isInsitu())
					h->getEvent()->setType(insitu1);
				if (h->getEvent()->isAdmission())
					h->getEvent()->setType(admission1);
			}
			else if (U < mod->getOutColParams()->transitionProb(prev,latent,time) + mod->getOutColParams()->transitionProb(prev,colonized,time))
			{
				eh->appendLink(mod->makeHistLink(f,u,atime,p,acquisition,0));
				eh->appendLink(mod->makeHistLink(f,u,atime,p,progression,0));
				if (h->getEvent()->isInsitu())
					h->getEvent()->setType(insitu2);
				if (h->getEvent()->isAdmission())
					h->getEvent()->setType(admission2);
			}
		}
	}

	static void randTestResult(UnitLinkedModel *mod, HistoryLink *h, Random *rand)
	{
		TestParams *tpar = 0;
		EventCode tres = h->getEvent()->getType();

		switch(tres)
		{
		case postest:
		case negtest:
		case possurvtest:
		case negsurvtest:
			tres = negsurvtest;
			tpar = mod->getSurveilenceTestParams();
			break;

		case posclintest:
		case negclintest:
			tres = negclintest;
			tpar = mod->getClinicalTestParams();
			break;

		default:
			break;
		}

		//if (rand->runif() < tpar->eventProb(h->getPState()->infectionStatus(),postest))
		if (rand->runif() < tpar->eventProb(h->getPState()->infectionStatus(),h->getPState()->onAbx(),postest))
		{
			if (tres == negsurvtest)
				tres = possurvtest;
			if (tres == negclintest)
				tres = posclintest;
		}

		h->getEvent()->setType(tres);
	}

	static double randTimeToEvent(UnitLinkedModel *mod, double atime, HistoryLink *ph, HistoryLink *uh, Random *rand)
	{
		double time = 0;

		switch(ph->getPState()->infectionStatus())
		{
		case uncolonized:
			time = atime + rand->rexp(mod->getInColParams()->eventRate(atime,acquisition,ph->getPState(),uh->getUState()));
			break;

		case latent:
			time = atime + rand->rexp(mod->getInColParams()->eventRate(atime,progression,ph->getPState(),uh->getUState()));
			break;

		case colonized:
			time = atime + rand->rexp(mod->getInColParams()->eventRate(atime,clearance,ph->getPState(),uh->getUState()));
			break;

		default:
			cerr << "CAN'T GET HERE\n";
			return 0;
			break;
		}

		if (time < atime)
		{
			cerr << "PROBLEM WITH REXP\n";
			exit(1);
		}

		return time ;
	}
};
