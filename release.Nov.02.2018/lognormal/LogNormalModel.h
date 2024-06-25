class LogNormalModel : public BasicModel
{
private:
	List *dumpers;

public:

	static LogNormalModel *readModel(istream &is, int nmetro, int forward, int cheat)
	{
		return readModel(is,0,nmetro,forward,cheat);
	}

	static LogNormalModel *readModel(istream &is, List *l, int nmetro, int forward, int cheat)
	{
		double timsig = 0.001;

		int maxline = 1000;
		char *c = new char[maxline];
		string sdump;
		double p, q, r;
		int up, uq, ur;
		
		int nstates = 0;
		int abxtest = 0;
		double abxd = 0.0;
		double abxl = 0.0;
		
		is >> sdump >> nstates;

		switch(nstates)
		{
		case 2:
		case 3:
			break;
		default:
			cerr << "Only 2 and 3 states models are allowed.\n";
			exit(1);
		}

		is.getline(c,maxline);

		is >> sdump >> abxtest;
		is.getline(c,maxline);

		is >> sdump >> abxd;
		is.getline(c,maxline);

		is >> sdump >> abxl;
		is.getline(c,maxline);
		is.getline(c,maxline);

		LogNormalModel *model = new LogNormalModel(l,nstates,abxtest,nmetro,forward,cheat);

		model->setAbxDelay(abxd);
		model->setAbxLife(abxl);

	// In situ parameters. 

		InsituParams *isp = model->getInsituParams();
		
		is >> sdump >> p >> up;
		is.getline(c,maxline);

		if (nstates == 2)
		{
			q = 0;
			uq = 0;
		}
		if (nstates == 3)
		{
			is >> sdump >> q >> uq;
			is.getline(c,maxline);
		}

		is >> sdump >> r >> ur;
		is.getline(c,maxline);
		is.getline(c,maxline);

		p = p+q+r;
		r = r/p;
		q = q/p;
		p = 1-q-r;
		up = ur;
		
		isp->set(p,q,r);
		isp->setPriors(p*up,q*up,r*ur);
		isp->setUpdate(up,uq,ur);

	// Surveilence test parameters.

		TestParams *stsp = model->getSurveilenceTestParams();

		is >> sdump >> p >> up;
		is.getline(c,maxline);

		if (nstates == 2)
		{
			q = 0;
			uq = 0;
		}
		if (nstates == 3)
		{
			is >> sdump >> q >> uq;
			is.getline(c,maxline);
		}

		is >> sdump >> r >> ur;
		is.getline(c,maxline);

		stsp->set(0,p,q,r);
		stsp->setPriors(0,p,up,q,uq,r,ur);
		stsp->setUpdate(0,up,uq,ur);

		if (abxtest)
		{
			is >> sdump >> p >> up;
			is.getline(c,maxline);
	
			if (nstates == 2)
			{
				q = 0;
				uq = 0;
			}
			if (nstates == 3)
			{
				is >> sdump >> q >> uq;
				is.getline(c,maxline);
			}
	
			is >> sdump >> r >> ur;
			is.getline(c,maxline);
	
			stsp->set(1,p,q,r);
			stsp->setPriors(1,p,up,q,uq,r,ur);
			stsp->setUpdate(1,up,uq,ur);
		}

		is.getline(c,maxline);


	//  Clinical test parameters.

		RandomTestParams *ctsp = (RandomTestParams *) model->getClinicalTestParams();

		if (ctsp != stsp)
		{
			is >> sdump >> p >> up;
			is.getline(c,maxline);

			if (nstates == 2)
			{
				q = 0;
				uq = 0;
			}
			if (nstates == 3)
			{
				is >> sdump >> q >> uq;
				is.getline(c,maxline);
			}

			is >> sdump >> r >> ur;
			is.getline(c,maxline);

			ctsp->set(p,q,r);
			ctsp->setPriors(p,up,q,uq,r,ur);
			ctsp->setUpdate(up,uq,ur);

			is >> sdump >> p >> up;
			is.getline(c,maxline);

			if (nstates == 2)
			{
				q = 1;
				uq = 0;
			}
			if (nstates == 3)
			{
				is >> sdump >> q >> uq;
				is.getline(c,maxline);
			}

			is >> sdump >> r >> ur;
			is.getline(c,maxline);
			is.getline(c,maxline);

			ctsp->setRates(p,q,r);	
			ctsp->setUpdateRates(up,uq,ur);
		}
		
	// Out of unit infection parameters.

		OutColParams *ocp = model->getOutColParams();

		is >> sdump >> p >> up;
		is.getline(c,maxline);

		if (nstates == 2)
		{
			q = 0;
			uq = 0;
		}
		if (nstates == 3)
		{
			is >> sdump >> q >> uq;
			is.getline(c,maxline);
		}

		is >> sdump >> r >> ur;
		is.getline(c,maxline);
		is.getline(c,maxline);

		if (nstates == 2)
		{
			ocp->set(p,r);
			ocp->setPriors(p,up,r,ur);
			ocp->setUpdate(up,ur);
		}
		if (nstates == 3)
		{
			ocp->set(p,q,r);
			ocp->setPriors(p,up,q,uq,r,ur);
			ocp->setUpdate(up,uq,ur);
		}

	// In unit infection parameters.

		LogNormalAbxICP *icp = (LogNormalAbxICP *) model->getInColParams();

		// Set parameters on rate scale using set() or on log-rate scale using setNormal().
		// These are not equivalent.
		// Formats are 
			// setNormal(paridi, paridj, log value, update?, prior mean log, prior var);
			// set(paridi, paridj, value, update?, prior mean value, equiv prior obs);

	// Acquisition: indexed by [0][*].
			// time effect: e(0) = 1 for no effect.
			// Set proposal variance small since scale for time is large.
		is >> sdump >> p >> up;
		is.getline(c,maxline);
		icp->set(0,0,p,up,p,up,timsig);

			// constant term: set at rough colonization rate eg 0.001;
		is >> sdump >> p >> up; 
		is.getline(c,maxline);

		if (l == 0)
		{
			icp->set(0,1,p,up,p,up);
		}
		else
		{
			icp->set(0,1,1,0,p,up);
			for (l->init(); l->hasNext(); )
				((MultiUnitAbxICP *)icp)->setUnit(0,l->next(),p,up,p,up);
		}

			// log total term. exp(0) = 1 for density dependent. exp(-1) = 0.3678794 for freq dependent.
		is >> sdump >> p >> up;
		is.getline(c,maxline);
		icp->set(0,2,p,up,p,up);

			// log ncol term. exp(1) = 2.718282 for density and frequency dependent.
		is >> sdump >> p >> up;
		is.getline(c,maxline);
		icp->set(0,3,p,up,p,up);

			// n colonized patient linear effect: e(0) = 1 for no effect.
		is >> sdump >> p >> up;
		is.getline(c,maxline);
		icp->set(0,4,p,up,p,up);

			// n abx colonized effect: e(0) = 1 for no effect.
		is >> sdump >> p >> up;
		is.getline(c,maxline);
		icp->set(0,5,p,up,p,up);

			// susseptible patient on Abx effect : e(0) = 1 for no effect.
		is >> sdump >> p >> up;
		is.getline(c,maxline);
		icp->set(0,6,p,up,p,up);

			// susseptible patient Ever on Abx effect : e(0) = 1 for no effect.
		is >> sdump >> p >> up;
		is.getline(c,maxline);
		icp->set(0,7,p,up,p,up);

	// Progression: indexed by [1][*].

		if (nstates == 2)
		{
			icp->set(1,0,0,0,0,0);
			icp->set(1,1,0,0,0,0);
			icp->set(1,2,0,0,0,0);
		}

		if (nstates == 3)
		{
				// base  rate
			is >> sdump >> p >> up;
			is.getline(c,maxline);
			icp->set(1,0,p,up,p,up);
				// abx effect
			is >> sdump >> p >> up;
			is.getline(c,maxline);
			icp->set(1,1,p,up,p,up);
				// ever abx effect
			is >> sdump >> p >> up;
			is.getline(c,maxline);
			icp->set(1,2,p,up,p,up);
		}

	// Clearance: indexed by [2][*].
			// base  rate
		is >> sdump >> p >> up;
		is.getline(c,maxline);
		icp->set(2,0,p,up,p,up);
			// abx effect
		is >> sdump >> p >> up;
		is.getline(c,maxline);
		icp->set(2,1,p,up,p,up);
			// ever abx effect
		is >> sdump >> p >> up;
		is.getline(c,maxline);
		icp->set(2,2,p,up,p,up);

		is.getline(c,maxline);

	// Abx parameters.

		AbxParams *abxp = model->getAbxParams();
	
		is >> sdump >> p >> up;
		is.getline(c,maxline);
		if (nstates == 2)
		{
			q = 0;
			uq = 0;
		}

		if (nstates == 3)
		{
			is >> sdump >> q >> uq;
			is.getline(c,maxline);
		}

		is >> sdump >>r >>ur;
		is.getline(c,maxline);

		abxp->set(p,q,r);
		abxp->setUpdate(up,uq,ur);
		abxp->setPriors(p,up,q,uq,r,ur);

	// Done.
		delete [] c;

		return model;
	}

	LogNormalModel(int nst, int abxtest, int nmetro, int fw = 0, int ch = 0) : BasicModel(nst,fw,ch)
	{
		icp = new LogNormalAbxICP(nst,0,nmetro);
		isp = new InsituParams(nstates);
		ocp = new OutColParams(nstates,nmetro);
		survtsp = new TestParamsAbx(nstates,abxtest);
		clintsp = new RandomTestParams(nstates);
		abxp = new AbxParams(nstates);
		dumpers = new List();
	}

	LogNormalModel(List *l, int nst, int abxtest, int nmetro, int fw = 0, int ch = 0) : BasicModel(nst,fw,ch)
	{
		icp = ( l == 0 ? new LogNormalAbxICP(nst,0,nmetro) :  new MultiUnitAbxICP(l,nst,0,nmetro) );

		isp = new InsituParams(nstates);
		ocp = new OutColParams(nstates,nmetro);
		survtsp = new TestParamsAbx(nstates,abxtest);
		clintsp = new RandomTestParams(nstates);
		abxp = new AbxParams(nstates);
		dumpers = new List();
	}

	~LogNormalModel()
	{
		delete abxp;
		delete ocp;
		delete isp;
		delete survtsp;
		delete clintsp;
		delete icp;

		for (dumpers->init(); dumpers->hasNext(); )
		{
			Object *x = dumpers->next();
			delete x;
		}
		delete dumpers;
	}

	virtual void write (ostream &os)
	{
		os << isp << "\t\t";
		os << survtsp << "\t\t";
		os << clintsp << "\t\t";
		os << ocp << "\t\t";
		os << icp << "\t\t";
		os << abxp << "\t\t";
	}

	virtual void writeHeader(ostream &os)
	{
		isp->writeHeader(os);
		os << "\t";
		survtsp->writeHeader(os);
		os << "\t";
		clintsp->writeHeader(os);
		os << "\t";
		ocp->writeHeader(os);
		os << "\t";
		icp->writeHeader(os);
		os << "\t";
		abxp->writeHeader(os);
		os << "\t";
	}

/*
	Customize required data.
*/

	virtual PatientState *makePatientState(Patient *p)
	{
		return p == 0 ? 0 : new AbxPatientState(p,nstates);
	}

	virtual LocationState *makeUnitState(Unit *u)
	{
		return u == 0 ? 0 : new AbxLocationState(u,nstates);
	}

	virtual void handleAbxDoses(HistoryLink *shead)
	{
		for (HistoryLink *l = shead; l != 0; ) //l = l->sNext())
		{
			if (l->getEvent()->getType() != abxdose)
			{
				l = l->sNext();
				continue;
			}

			double ont = l->getEvent()->getTime();
			ont += getAbxDelay();
			HistoryLink *onpnext = l;
			for ( ; onpnext != 0; onpnext = onpnext->pNext())
				if (onpnext->getEvent()->getTime() >= ont)
					break;
		
			double offt = l->getEvent()->getTime();
			offt += getAbxLife();

			HistoryLink *offpnext = l;
			for ( ; offpnext != 0; offpnext = offpnext->pNext())
				if (offpnext->getEvent()->getTime() >= offt)
					break;

			if (offpnext == onpnext && onpnext != 0 && onpnext->getEvent()->isAdmission())
			{
				onpnext = 0;
				offpnext = 0;
			}

			if (offpnext)
			{
				HistoryLink *snext = 0;

				if (offpnext->getEvent()->isAdmission())
				{
					offpnext = offpnext->pPrev();
					offt = offpnext->getEvent()->getTime();
					snext = offpnext;
				}
				else
				{
					for (snext = l; snext != 0; snext = snext->sNext())
						if (snext->getEvent()->getTime() >= offt)
							break;
				}
	
				Event *off = new Event(offpnext->getEvent()->getFacility(),offpnext->getEvent()->getUnit(),offt,offpnext->getEvent()->getPatient(),abxoff);
				HistoryLink *loff = new HistoryLink
						(
							off,
							makeSystemState(),
							makeFacilityState(off->getFacility()),
							makeUnitState(off->getUnit()),
							makePatientState(off->getPatient())
						);
	
				loff->insertAsap(snext);
				dumpers->append(off);
			}

			if (onpnext)
			{
				HistoryLink *snext = 0;

				if (onpnext->getEvent()->isAdmission())
				{
					onpnext = onpnext->pPrev();
					ont = onpnext->getEvent()->getTime();
					snext = onpnext;
				}
				else
				{
					for (snext = l; snext != 0; snext = snext->sNext())
						if (snext->getEvent()->getTime() >= ont)
							break;
				}
	
				Event *on = new Event(onpnext->getEvent()->getFacility(),onpnext->getEvent()->getUnit(),ont,onpnext->getEvent()->getPatient(),abxon);
				HistoryLink *lon = new HistoryLink
						(
							on,
							makeSystemState(),
							makeFacilityState(on->getFacility()),
							makeUnitState(on->getUnit()),
							makePatientState(on->getPatient())
						);
	
				lon->insertAsap(snext);
				dumpers->append(on);
			}

			HistoryLink *ll = l;
			l = l->sNext();
			ll->remove();
			delete ll;
		}
	}

	virtual int needEventType(EventCode e)
	{
		if (cheating)
		{
			switch(e)
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
			case acquisition:
			case progression:
			case clearance:
		//	case abxon:
		//	case abxoff:
			case abxdose:
				return 1;
			default:
				return 0;
			}
		}
		else
		{	
			switch(e)
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
		//	case abxon:
		//	case abxoff:
			case abxdose:
				return 1;
			default:
				return 0;
			}
		}
	}
};
