#ifndef ALUN_MODELING_CONTRAINTEDSIMULATOR_H
#define ALUN_MODELING_CONTRAINTEDSIMULATOR_H

#include "../infect/infect.h"
#include "UnitLinkedModel.h"

class ConstrainedSimulator : public Object, infect::EventCoding, infect::InfectionCoding
{
protected:

	static inline int stateAfterEvent(int nstates, EventCode ev)
	{
		if (nstates == 2)
		{
			switch(ev)
			{
			case acquisition: return 1;
			case clearance: return 0;
			default:
				return -1;
			}
		}
		if (nstates == 3)
		{
			switch(ev)
			{
			case acquisition: return 1;
			case progression: return 2;
			case clearance: return 0;
			default:
				return -1;
			}
		}
		return -1;
	}

	static inline EventCode eventOutOfState(int nstates, int i)
	{
		if (nstates == 2)
		{
			switch(i)
			{
			case 0: return acquisition;
			case 1: return clearance;
			default:
				return nullevent;
			}
		}
		if (nstates == 3)
		{
			switch(i)
			{
			case 0: return acquisition;
			case 1: return progression;
			case 2: return clearance;
			default:
				return nullevent;
			}
		}
		return nullevent;
	}

	static void getProposal(UnitLinkedModel *mod, infect::EpisodeHistory *h, int *nsim, double **times, int **states)
	{
		vector<int> vs;
		vector<double> vt;
		vs.push_back(0);
		vt.push_back(h->admissionTime());

		for (infect::HistoryLink *l = h->getProposalHead(); l != 0; l = l->hNext())
		{
			if (!l->isLinked())
			{
				vs[0] = stateAfterEvent(mod->getNStates(),l->getEvent()->getType());
			}
			else
			{
				vs.push_back(stateAfterEvent(mod->getNStates(),l->getEvent()->getType()));
				vt.push_back(l->getEvent()->getTime());
			}
		}

		*nsim = vs.size();
		*states = new int[*nsim];
		*times = new double[*nsim];
		for (int i=0; i<(*nsim); i++)
		{
			(*states)[i] = vs[i];
			(*times)[i] = vt[i];
		}
	}

	static void putProposal(UnitLinkedModel *mod, infect::EpisodeHistory *h, int nsim, double *times, int *states)
	{
	    infect::Facility *f = h->admissionLink()->getEvent()->getFacility();
	    infect::Unit *u = h->admissionLink()->getEvent()->getUnit();
	    infect::Patient *p = h->admissionLink()->getEvent()->getPatient();

		if (mod->getNStates() == 2)
		{
			if (states[0] == 1)
			{
				h->proposeSwitch(mod->makeHistLink(f,u,p,times[0],acquisition,0));
			}
		}
		if (mod->getNStates() == 3)
		{
			if (states[0] == 1)
			{
				h->proposeSwitch(mod->makeHistLink(f,u,p,times[0],acquisition,0));
			}
			if (states[0] == 2)
			{
				h->proposeSwitch(mod->makeHistLink(f,u,p,times[0],acquisition,0));
				h->proposeSwitch(mod->makeHistLink(f,u,p,times[0],progression,0));
			}
		}

		for (int i = 1; i < nsim ; i++)
			h->proposeSwitch(mod->makeHistLink(f,u,p,times[i],eventOutOfState(mod->getNStates(),states[i-1]),1));
	}

	static Markov *getMarkovProcess(UnitLinkedModel *mod, infect::HistoryLink *p, Random *rand, int *nalloc, double **mytime, bool **mydoit, double ***myS, double ****myQ)
	{
	    infect::Patient *pat = p->getEvent()->getPatient();
		int n = 0;
		infect::HistoryLink *l;
		bool first = true;

		for (l=p, n=0; l!=0; )
		{
			n++;
			l = (l->getEvent()->getPatient()==pat && l->getEvent()->getType()==discharge ? l->pNext() : l->uNext());
		}

		double *time = new double[n];
		bool *doit = new bool[n];
		double **S = new double*[n];
		double ***Q = new double **[n];

		for
		(
			first=true, l=p, n=0;
			l!=0;
			l = (l->getEvent()->getPatient()==pat && l->getEvent()->getType()==discharge ? l->pNext() : l->uNext())
		)
		{
			doit[n] = true;
			S[n] = 0;
			Q[n] = 0;

			if (l->getEvent()->getPatient() == pat)
			{
				switch(l->getEvent()->getType())
				{
				case insitu:
				case insitu0:
				case insitu1:
				case insitu2:
					if (first)
					{
						S[n] = mod->getInsituParams()->statusProbs();
						first = false;
					}
					Q[n] = mod->getInColParams()->rateMatrix(l->getEvent()->getTime(),l->getPState(),l->getUState());
					break;

				case admission:
				case admission0:
				case admission1:
				case admission2:
					if (first)
					{
						S[n] = mod->getOutColParams()->equilibriumProbs();
						first = false;
					}
					Q[n] = mod->getInColParams()->rateMatrix(l->getEvent()->getTime(),l->getPState(),l->getUState());
					break;

				case postest:
				case possurvtest:
				case negtest:
				case negsurvtest:
					S[n] =  mod->getSurveilenceTestParams()->resultProbs(l->getPState()->onAbx(),l->getEvent()->getType());
					break;
				case posclintest:
				case negclintest:
					S[n] =  mod->getClinicalTestParams()->resultProbs(l->getPState()->onAbx(),l->getEvent()->getType());
					break;

				case discharge:
					Q[n] = mod->getOutColParams()->rateMatrix();
					doit[n] = false;
					break;

				default:
					continue;
				}
			}
			else
			{
				switch(l->getEvent()->getType())
				{
				case acquisition:
					S[n] = mod->getInColParams()->acquisitionRates(l->getEvent()->getTime(),l->pPrev()->getPState(),l->uPrev()->getUState());
				case progression:
				case clearance:
				case admission:
				case discharge:
					Q[n] = mod->getInColParams()->rateMatrix(l->getEvent()->getTime(),l->getPState(),l->getUState());
					break;
				default:
					continue;
				}
			}

			time[n++] = l->getEvent()->getTime();
		}

		*mytime = time;
		*mydoit = doit;
		*myS = S;
		*myQ = Q;
		*nalloc = n;

		return new Markov(mod->getNStates(),n,time,Q,S,doit,rand);
	}

public:

	static void sampleEpisodes(UnitLinkedModel *mod, infect::SystemHistory *h, int max, Random *rand)
	{
		for (Map *p = h->getPatientHeads(); p->hasNext(); ){
			sampleHistory(mod,h,(infect::HistoryLink *)p->nextValue(),max,rand);
	    }
	}

	static void sampleHistory(UnitLinkedModel *mod, infect::SystemHistory *hist, infect::HistoryLink *plink, int max, Random *rand)
	{

	    infect::Patient *pat = plink->getEvent()->getPatient();
		int neps = 0;
		infect::EpisodeHistory **eh = hist->getPatientHistory(pat,&neps);

		double oldloglike = 0;
		double oldpropprob = 0;
		double newloglike = 0;
		double newpropprob = 0;

		int *on = new int[neps];
		int *nn = new int[neps];
		int **os = new int*[neps];
		int **ns = new int*[neps];
		double **ot = new double*[neps];
		double **nt = new double*[neps];

		oldloglike = mod->logLikelihood(pat,plink);

		for (int i=0; i<neps; i++)
		{
			eh[i]->unapply();
			eh[i]->installProposal();
			getProposal(mod,eh[i],&(on[i]),&(ot[i]),&(os[i]));
			eh[i]->installProposal();
		}

		int nalloc = 0;
		double *mytime = 0;
		double **myS = 0;
		double ***myQ = 0;
		bool *mydoit = 0;

		Markov *mark = getMarkovProcess(mod,plink,rand,&nalloc,&mytime,&mydoit,&myS,&myQ);

		oldpropprob = mark->logProcessProb(neps,on,ot,os);

		newpropprob = mark->simulateProcess(neps,nn,nt,ns);

		for (int i=0; i<neps; i++)
		{
			putProposal(mod,eh[i],nn[i],nt[i],ns[i]);
			eh[i]->installProposal();
			eh[i]->apply();
		}

		newloglike = mod->logLikelihood(pat,plink);

		double accept = newloglike-oldloglike;
		double logU = 0;
		if (!max)
		{
			accept += oldpropprob - newpropprob;
			logU = log(rand->runif());
		}

		if (logU <= accept)
		{
			for (int i=0; i<neps; i++)
				eh[i]->clearProposal();
		}
		else
		{
			for (int i=0; i<neps; i++)
			{
				eh[i]->unapply();
				eh[i]->installProposal();
				eh[i]->apply();
				eh[i]->clearProposal();
			}
		}

		cleanFree(&os,neps);
		cleanFree(&ns,neps);
		cleanFree(&ot,neps);
		cleanFree(&nt,neps);
		delete [] on;
		delete [] nn;
		delete [] eh;
		delete [] mytime;
		delete [] mydoit;
		cleanFree(&myS,nalloc);
		cleanFree(&myQ,nalloc,mod->getNStates());
		delete mark;
	}

	static void initEpisodeHistory(UnitLinkedModel *mod, infect::EpisodeHistory *eh, bool haspostest)
	{
		if (mod->isCheating())
		{
			cheatInitEpisodeHistory(mod,eh);
			return;
		}

		if (haspostest)
		{
		    infect::Facility *f = eh->admissionLink()->getEvent()->getFacility();
		    infect::Unit *u = eh->admissionLink()->getEvent()->getUnit();
		    infect::Patient *p = eh->admissionLink()->getEvent()->getPatient();
			double admit = eh->admissionTime();

			switch(mod->getNStates())
			{
			case 2: eh->proposeSwitch(mod->makeHistLink(f,u,p,admit,acquisition,0));
				break;
			case 3: eh->proposeSwitch(mod->makeHistLink(f,u,p,admit,acquisition,0));
				eh->proposeSwitch(mod->makeHistLink(f,u,p,admit,progression,0));
				break;
			}

			eh->installProposal();
			eh->apply();
		}
	}

	static void cheatInitEpisodeHistory(UnitLinkedModel *mod, infect::EpisodeHistory *eh)
	{
// This is ok as long as the Sampler that calls it does a clean up
// step. But it needs work.

		infect::Facility *f = eh->admissionLink()->getEvent()->getFacility();
	    infect::Unit *u = eh->admissionLink()->getEvent()->getUnit();
	    infect::Patient *p = eh->admissionLink()->getEvent()->getPatient();
		double admit = eh->admissionTime();

		if (mod->getNStates() == 2)
		{
			switch(eh->admissionLink()->getEvent()->getType())
			{
			case insitu2:
			case admission2:
				eh->proposeSwitch(mod->makeHistLink(f,u,p,admit,acquisition,0));
				break;
			default:
				break;
			}
		}

		if (mod->getNStates() == 3)
		{
			switch(eh->admissionLink()->getEvent()->getType())
			{
			case insitu1:
			case admission1:
				eh->proposeSwitch(mod->makeHistLink(f,u,p,admit,acquisition,0));
				break;

			case insitu2:
			case admission2:
				eh->proposeSwitch(mod->makeHistLink(f,u,p,admit,acquisition,0));
				eh->proposeSwitch(mod->makeHistLink(f,u,p,admit,progression,0));
				break;

			default:
				break;
			}
		}

		for (infect::HistoryLink *h = eh->admissionLink(); h != eh->dischargeLink(); h = h->pNext())
		{
			if (!h->getEvent()->isCollonizationEvent())
				continue;
			eh->proposeSwitch(h);
		}

		eh->installProposal();
	//	eh->apply(); Don't do this. It applied events already in there.
	}
};
#endif //ALUN_MODELING_CONTRAINTEDSIMULATOR_H
