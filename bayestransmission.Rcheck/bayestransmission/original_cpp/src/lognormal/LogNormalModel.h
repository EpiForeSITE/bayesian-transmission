class LogNormalModel : public BasicModel
{
protected:

	bool abxbyonoff;

	LogNormalModel(int nst, int fw, int ch) : BasicModel(nst,fw,ch)
	{
		abxbyonoff = 0;
		dumpers = new List();
	}

public:

	LogNormalModel(int nst, int nmetro, int fw, int ch) : BasicModel(nst,fw,ch)
	{
		abxbyonoff = 0;
		dumpers = new List();

		isp = new InsituParams(nstates);
		survtsp = new TestParams(nstates);
		clintsp = new RandomTestParams(nstates);
		ocp = new OutColParams(nstates,nmetro);
		icp = new LNMassActionICP(nst,0);
		abxp = new AbxParams(nstates);
	}

	~LogNormalModel()
	{
		if (abxp != 0)
			delete abxp;
		delete ocp;
		delete isp;
		if (clintsp != 0 && clintsp != survtsp)
			delete clintsp;
		delete survtsp;
		delete icp;

		for (dumpers->init(); dumpers->hasNext(); )
		{
			Object *x = dumpers->next();
			delete x;
		}
		delete dumpers;
	}

	virtual int needEventType(EventCode e)
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
			return 1;

		case abxdose:
			return ( abxbyonoff ? 0 : 1 );

		case abxon:
		case abxoff:
			return ( abxbyonoff ? 1 : 0);

		case acquisition:
		case progression:
		case clearance:
			return cheating ? 1 : 0;

		default:
			return 0;
		}
	}

	virtual PatientState *makePatientState(Patient *p)
	{
		return p == 0 ? 0 : new AbxPatientState(p,nstates);
	}

	virtual LocationState *makeUnitState(Unit *u)
	{
		return u == 0 ? 0 : new AbxLocationState(u,nstates);
	}

	virtual void read(istream &is)
	{
		string sdump;

		is >> sdump >> abxbyonoff;
		skipLine(is);

		if (!abxbyonoff)
		{
			double abx = 0.0;
			is >> sdump >> abx;
			setAbxDelay(abx);
			skipLine(is);
			is >> sdump >> abx;
			setAbxLife(abx);
			skipLine(is);
		}

		skipLine(is);

		readInsituParams(getInsituParams(),is);
		skipLine(is);

		TestParams *stsp = getSurveilenceTestParams();
		readTestParams(stsp,is);
		skipLine(is);

		RandomTestParams *ctsp = (RandomTestParams *) getClinicalTestParams();
		if (ctsp != 0 && ctsp != stsp)
		{
			readRandomTestParams(ctsp,is);
			skipLine(is);
		}

		readOutColParams(getOutColParams(),is);
		skipLine(is);

		readInColParams((LogNormalICP *)getInColParams(),is);
		skipLine(is);

		AbxParams *abxp = getAbxParams();
		if (abxp != 0)
		{
			readAbxParams(abxp,is);
			skipLine(is);
		}
	}

	virtual void handleAbxDoses(HistoryLink *shead)
	{
		if (abxbyonoff)
			return;

		// Loop through all events picking out abxdose events.
		for (HistoryLink *l = shead; l != 0; )
		{
			if (l->getEvent()->getType() != abxdose)
			{
				l = l->sNext();
				continue;
			}

			// Find on and off times corresponding to dose event.
			// Find first events folowing these times.
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

			// Discard on and off times implied to be out of unit.
			if (offpnext == onpnext && onpnext != 0 && onpnext->getEvent()->isAdmission())
			{
				onpnext = 0;
				offpnext = 0;
			}

			// Create off abx event with fix if it's implied to be out of unit.
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

				Event *e = offpnext->getEvent();
				Event *off = new Event(e->getFacility(),e->getUnit(),offt,e->getPatient(),abxoff);
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

			// Crate on abx event with fix if its implied to be out of unit.
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

				Event *e = onpnext->getEvent();
				Event *on = new Event(e->getFacility(),e->getUnit(),ont,e->getPatient(),abxon);
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

			// Remove the abx dose event.
			HistoryLink *ll = l;
			l = l->sNext();
			ll->remove();
			delete ll;
		}

		// Keep only on/off events that change the state.
		Map *m = new Map();

		for (HistoryLink *l = shead; l != 0; )
		{
			HistoryLink *ll = l;
			l = l->sNext();

			if (ll->getEvent()->getType() != abxon && ll->getEvent()->getType() != abxoff)
				continue;

			Patient *p = ll->getEvent()->getPatient();

			Integer *i = (Integer *) m->get(p);
			if (i == 0)
			{
				i = new Integer(0);
				m->put(p,i);
			}

			switch(ll->getEvent()->getType())
			{
			case abxon:
				if (++(i->value) != 1)
				{
					ll->remove();
					delete ll;
				}
				break;

			case abxoff:
				if (--(i->value) != 0)
				{
					ll->remove();
					delete ll;
				}
				break;

			default:
				cerr << "Can't get here in handle abx doses\n";
				break;
			}
		}

		for (m->init(); m->hasNext(); )
			delete m->nextValue();
		delete m;
	}

protected:

	static void skipLine(istream &is)
	{
		char c;
		do
		{
			c = is.get();
		}
		while (c != '\n' && c != EOF);
	}

	virtual void readInColParams(LogNormalICP *icp, istream &is)
	{
		string sdump;
		double p;
		double up;

		for (int i=0; i<3; i++)
		{
			if (i != 1)
			{
				for (int j=0; j<icp->nParam(i); j++)
				{
					is >> sdump >> p >> up;
					skipLine(is);
					icp->set(i,j,p,(up>0),p,up);
				}
			}
			else
			{
				if (icp->getNStates() == 2)
				{
					for (int j=0; j<icp->nParam(i); j++)
						icp->set(i,j,0,0,0,0);
				}

				if (icp->getNStates() == 3)
				{
					for (int j=0; j<icp->nParam(i); j++)
					{
						is >> sdump >> p >> up;
						skipLine(is);
						icp->set(i,j,p,(up>0),p,up);
					}
				}
			}
		}
	}

	virtual void readInsituParams(InsituParams *isp, istream &is)
	{
		string sdump;
		double p, q, r;
		double up, uq, ur;

		is >> sdump >> p >> up;
		skipLine(is);

		if (isp->getNStates() == 2)
		{
			q = 0;
			uq = 0;
		}
		if (isp->getNStates() == 3)
		{
			is >> sdump >> q >> uq;
			skipLine(is);
		}

		is >> sdump >> r >> ur;
		skipLine(is);

		p = p+q+r;
		r = r/p;
		q = q/p;
		p = 1-q-r;
		up = ur;

		isp->set(p,q,r);
		isp->setPriors(p*up,q*up,r*ur);
		isp->setUpdate((up>0),(uq>0),(ur>0));
	}

	virtual void readTestParams(TestParams *stsp, istream &is)
	{
		string sdump;
		double up, uq, ur;
		double p, q, r;

		is >> sdump >> p >> up;
		skipLine(is);

		if (stsp->getNStates() == 2)
		{
			q = 0;
			uq = 0;
		}
		if (stsp->getNStates() == 3)
		{
			is >> sdump >> q >> uq;
			skipLine(is);
		}

		is >> sdump >> r >> ur;
		skipLine(is);

		stsp->set(0,p,(up>0),p,up);
		stsp->set(1,q,(uq>0),q,uq);
		stsp->set(2,r,(ur>0),r,ur);

		//stsp->set(0,p,q,r);
		//stsp->setPriors(0,p,up,q,uq,r,ur);
		//stsp->setUpdate(0,(up>0),(uq>0),(ur>0));
	}

	virtual void readRandomTestParams(RandomTestParams *ctsp, istream &is)
	{
		string sdump;
		double up, uq, ur;
		double p, q, r;

		is >> sdump >> p >> up;
		skipLine(is);

		if (ctsp->getNStates() == 2)
		{
			q = 0;
			uq = 0;
		}
		if (ctsp->getNStates() == 3)
		{
			is >> sdump >> q >> uq;
			skipLine(is);
		}

		is >> sdump >> r >> ur;
		skipLine(is);

		ctsp->set(0,p,(up>0),p,up);
		ctsp->set(1,q,(uq>0),q,uq);
		ctsp->set(2,r,(ur>0),r,ur);

		is >> sdump >> p >> up;
		skipLine(is);

		if (ctsp->getNStates() == 2)
		{
			q = 1;
			uq = 0;
		}
		if (ctsp->getNStates() == 3)
		{
			is >> sdump >> q >> uq;
			skipLine(is);
		}

		is >> sdump >> r >> ur;
		skipLine(is);

		ctsp->set(1,0,p,(up>0),p,up);
		ctsp->set(1,1,q,(uq>0),q,uq);
		ctsp->set(1,2,r,(ur>0),r,ur);
	}

	virtual void readOutColParams(OutColParams *ocp, istream &is)
	{
		string sdump;
		double up, uq, ur;
		double p, q, r;

		is >> sdump >> p >> up;
		skipLine(is);

		if (ocp->getNStates() == 2)
		{
			q = 0;
			uq = 0;
		}
		if (ocp->getNStates() == 3)
		{
			is >> sdump >> q >> uq;
			skipLine(is);
		}

		is >> sdump >> r >> ur;
		skipLine(is);

		ocp->set(0,p,(up>0),p,up);
		ocp->set(1,q,(uq>0),q,uq);
		ocp->set(2,r,(ur>0),r,ur);
/*
		if (ocp->getNStates() == 2)
		{
			ocp->set(p,r);
			ocp->setPriors(p,up,r,ur);
			ocp->setUpdate((up>0),(ur>0));
		}
		if (ocp->getNStates() == 3)
		{
			ocp->set(p,q,r);
			ocp->setPriors(p,up,q,uq,r,ur);
			ocp->setUpdate((up>0),(uq>0),(ur>0));
		}
*/
	}

	virtual void readAbxParams(AbxParams *abxp, istream &is)
	{
		string sdump;
		double up, uq, ur;
		double p, q, r;

		is >> sdump >> p >> up;
		skipLine(is);
		if (abxp->getNStates() == 2)
		{
			q = 0;
			uq = 0;
		}

		if (abxp->getNStates() == 3)
		{
			is >> sdump >> q >> uq;
			skipLine(is);
		}

		is >> sdump >>r >>ur;
		skipLine(is);

		abxp->set(0,p,(up>0),p,up);
		abxp->set(1,q,(uq>0),q,uq);
		abxp->set(2,r,(ur>0),r,ur);

/*
		abxp->set(p,q,r);
		abxp->setPriors(p,up,q,uq,r,ur);
		abxp->setUpdate((up>0),(uq>0),(ur>0));
*/
	}

private:

	List *dumpers;

};
