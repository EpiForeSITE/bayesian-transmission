#ifndef ALUN_MODELING_UNITLINKEDMODEL_H
#define ALUN_MODELING_UNITLINKEDMODEL_H

#include "../infect/infect.h"
#include "InsituParams.h"
#include "OutColParams.h"
#include "TestParams.h"
#include "InColParams.h"
#include "AbxParams.h"



/*
	Manages models where it is only required that the
	augmented data events are linked into the Patient and Unit
	history lists, not Facility or System lists.
*/

class UnitLinkedModel : public infect::Model
{
protected:

	int nstates;
	int forwardEnabled;

	InsituParams *isp;
	OutColParams *ocp;
	TestParams *survtsp;
	TestParams *clintsp;
	InColParams *icp;
	AbxParams *abxp;

public:

	UnitLinkedModel(int ns, int fw, int ch)
	{
		setAbxLife(1.5);
		setAbxDelay(0.0);
		nstates = ns;
		forwardEnabled = fw;
		cheating = ch;

		isp = 0;
		ocp = 0;
		survtsp = 0;
		clintsp = 0;
		icp = 0;
		abxp = 0;
	}


	virtual double logLikelihood(infect::SystemHistory *hist)
	{
		double xtot = 0;
		for (Map *h = hist->getUnitHeads(); h->hasNext(); )
		{
			double utot = 0;
			for (infect::HistoryLink *l = (infect::HistoryLink *) h->nextValue(); l != 0; l=l->uNext())
				utot += logLikelihood(l);
			xtot += utot;
		}
		return xtot;
	}

public:

	inline int isForwardEnabled()
	{
		return forwardEnabled;
	}

	inline int getNStates() const
	{
		return nstates;
	}

	inline InsituParams *getInsituParams()
	{
		return isp;
	}

	inline OutColParams *getOutColParams()
	{
		return ocp;
	}

	inline TestParams *getSurveilenceTestParams()
	{
		return survtsp;
	}

	inline TestParams *getClinicalTestParams()
	{
		return clintsp;
	}

	inline InColParams *getInColParams()
	{
		return icp;
	}

	inline void setInColParams(InColParams *p)
	{
		icp = p;
	}

	inline AbxParams *getAbxParams()
	{
		return abxp;
	}

	inline void setAbxParams(AbxParams *p)
	{
		abxp = p;
	}

	virtual infect::HistoryLink *makeHistLink(infect::Facility *f, infect::Unit *u, infect::Patient *p, double time, EventCode type, int linked)
	{
		return new infect::HistoryLink
		(
			new infect::Event(f,u,time,p,type),
			makeSystemState(),
			makeFacilityState(f),
			makeUnitState(u),
			makePatientState(p),
			linked
		);
	}

		virtual string header()
	{
		stringstream os;
		os << isp->header();
		os << "\t" << survtsp->header();
		if (clintsp && clintsp != survtsp)
			os << "\t" << clintsp->header();
		os << "\t" << ocp->header();
		os << "\t" << icp->header();
		if (abxp)
			os << "\t" << abxp->header();
		return os.str();
	}

    virtual void write (ostream &os)
	{
		os << isp;
		os << "\t" << survtsp;
		if (clintsp && clintsp != survtsp)
			os << "\t" << clintsp;
		os << "\t" << ocp;
		os << "\t" << icp;
		if (abxp != 0)
			os << "\t" << abxp;
	}
    /*
	virtual void writeHeader(ostream &os)
	{
		isp->writeHeader(os);
		os << "\t";
		survtsp->writeHeader(os);
		os << "\t";
		if (clintsp != survtsp)
		{
			clintsp->writeHeader(os);
			os << "\t";
		}

		ocp->writeHeader(os);
		os << "\t";
		icp->writeHeader(os);
		os << "\t";
		if (abxp != 0)
		{
			abxp->writeHeader(os);
			os << "\t";
		}
	}
    */
//protected:

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
		case abxdose:
		//case abxon:
		//case abxoff:
			return 1;
		case acquisition:
		case progression:
		case clearance:
			return cheating;
		default:
			return 0;
		}
	}

	virtual infect::LocationState *makeSystemState()
	{
		return ( forwardEnabled ? new infect::SetLocationState(0,nstates) : 0 );
	}

	virtual infect::LocationState *makeFacilityState(infect::Facility *f)
	{
		return 0;
	}

	virtual infect::LocationState *makeUnitState(infect::Unit *u)
	{
		return u == 0 ? 0 : new infect::CountLocationState(u,nstates);
	}

	virtual infect::PatientState *makePatientState(infect::Patient *p)
	{
		return p == 0 ? 0 : new infect::PatientState(p,nstates);
	}

	virtual infect::EpisodeHistory *makeEpisodeHistory(infect::HistoryLink *a, infect::HistoryLink *d)
	{
		if (forwardEnabled)
			return new infect::SystemEpisodeHistory(a,d);
		else
			return new infect::UnitEpisodeHistory(a,d);
	}

	void countUnitStats(infect::HistoryLink *l)
	{
	    infect::HistoryLink *prev = l;

		for (infect::HistoryLink *h = l->uNext() ; h != 0; h = h->uNext())
		{
			icp->countGap(prev,h);
			survtsp->countGap(prev,h);
			if (clintsp && clintsp != survtsp)
				clintsp->countGap(prev,h);
			if (abxp != 0)
				abxp->countGap(prev,h);

			if (h->isHidden())
				break;

			switch(h->getEvent()->getType())
			{
			case insitu:
			case insitu0:
			case insitu1:
			case insitu2:
				isp->count(h);
				break;

			case admission:
			case admission0:
			case admission1:
			case admission2:
				ocp->count(h);
				break;

			case negsurvtest:
			case possurvtest:
				survtsp->count(h);
				break;

			case negclintest:
			case posclintest:
				if (clintsp)
				    clintsp->count(h);
				break;

			case acquisition:
			case progression:
			case clearance:
				icp->count(h);
				break;

			case abxon:
				if (abxp != 0)
					abxp->count(h);
				break;

			case abxdose:
			case abxoff:
			case discharge:
			case marker:
			case start:
			case stop:
			case isolon:
			case isoloff:
				break;

			default:
				cerr << "Event not handled " << h->getEvent() << "\n";
				break;
			}

			prev = h;
		}
	}

	virtual double logLikelihood(infect::EpisodeHistory *h)
	{
		return logLikelihood(h,0);
	}

	virtual double logLikelihood(infect::EpisodeHistory *h, int opt)
	{
		double x = 0;
		for (infect::HistoryLink *l = h->admissionLink(); ; l = l->uNext())
		{
			x += logLikelihood(l);
			if (l == h->dischargeLink())
			{
				if (l->pNext() != 0)
				{
					l = l->pNext();
					x += logLikelihood(l,0);
				}
				break;
			}
		}
		return x;
	}

	virtual double logLikelihood(infect::Patient *pat, infect::HistoryLink *h)
	{
		return logLikelihood(pat,h,0);
	}

	virtual double logLikelihood(infect::Patient *pat, infect::HistoryLink *h, int opt)
	{
		double x = 0;

		for (infect::HistoryLink *l = h; l != 0; )
		{
		    infect::Event *e = l->getEvent();
			if (e->getPatient() == pat && (e->isAdmission() || e->isInsitu()))
				x += logLikelihood(l,0);
			else
				x += logLikelihood(l,1);

			if (e->getPatient() == pat && e->getType() == discharge)
				l = l->pNext();
			else
				l = l->uNext();
		}

		return x;
	}

	virtual double logLikelihood(infect::HistoryLink *h)
	{
		return logLikelihood(h,1);
	}

	virtual double logLikelihood(infect::HistoryLink *h, int dogap)
	{
		switch(h->getEvent()->getType())
		{
		case start:
		case stop:
			return 0;
		default:
			break;
		}

	    infect::HistoryLink *prev = h->uPrev();
		double x = 0;

		if (dogap)
		{
			x += icp->logProbGap(prev,h);
			x += survtsp->logProbGap(prev,h);
			if (clintsp && clintsp != survtsp)
				x += clintsp->logProbGap(prev,h);
			if (abxp != 0)
				x += abxp->logProbGap(prev,h);
		}

		if (h->isHidden())
			return x;

		switch(h->getEvent()->getType())
		{
		case insitu:
		case insitu0:
		case insitu1:
		case insitu2:
			x += isp->logProb(h);
			break;

		case admission:
		case admission0:
		case admission1:
		case admission2:
			x += ocp->logProb(h);
			break;

		case acquisition:
		case progression:
		case clearance:
			x += icp->logProb(h);
			break;

		case negsurvtest:
		case possurvtest:
			x += survtsp->logProb(h);
			break;

		case negclintest:
		case posclintest:
			if (clintsp)
			    x += clintsp->logProb(h);
			break;

		case abxon:
			if (abxp != 0)
				x += abxp->logProb(h);
			break;

		case abxdose:
		case discharge:
		case abxoff:
		case marker:
		case start:
		case stop:
		case isolon:
		case isoloff:
			break;

		default:
			cerr << "Event not handled " << h->getEvent() << "\n";
			break;
		}

		return x;
	}

	void update(infect::SystemHistory *hist, Random *r)
	{
		update(hist,r,0);
	}

	void update(infect::SystemHistory *hist, Random *r, int max)
	{
		isp->initCounts();
		survtsp->initCounts();
		if (clintsp && clintsp != survtsp)
			clintsp->initCounts();
		icp->initCounts();
		ocp->initCounts();
		if (abxp != 0)
			abxp->initCounts();

		for (Map *h = hist->getUnitHeads(); h->hasNext(); )
			countUnitStats((infect::HistoryLink *)h->nextValue());

		isp->update(r,max);
		icp->update(r,max);
		survtsp->update(r,max);
		if (clintsp && clintsp != survtsp)
			clintsp->update(r,max);
		ocp->update(r,max);
		if (abxp != 0)
			abxp->update(r,max);
	}
};
#endif // ALUN_MODELING_UNITLINKEDMODEL_H
