
#include <stdio.h>
#include <iostream>

#include "infect.h"

int main(int argc, char *argv[])
{
	try
	{
		stringstream errstream (stringstream::out);
		System *data = new System(cin,errstream);
		SystemHistory *hist = new SystemHistory(data);

		Map *postest = new Map();
		Map *evpostest = new Map();

		int *ntot = new int[1000];
		int *npat = new int[1000];
		int *npos = new int[1000];
		int *nevpos = new int[1000];
		for (int i=0; i<1000; i++)
			ntot[i] = npat[i] = npos[i] = nevpos[i] = 0;

		for (HistoryLink *h = hist->getSystemHead(); h != 0; h = h->sNext())
		{
			double time = h->getEvent()->getTime();
			Patient *pat = h->getEvent()->getPatient();
			Unit *u = h->getEvent()->getUnit();
			Facility *f = h->getEvent()->getFacility();
			int i = ( u == 0 ? 0  : u->getId());

			switch(h->getEvent()->getType())
			{
			case EventCoding::insitu: 
				cout << f << "\t" << u << "\t" << time << "\t" << npat[i] << "\t" << npos[i] << "\t" << nevpos[i] << "\t" << ntot[i] << "\n";
				ntot[i]++;
				npat[i]++;
				cout << f << "\t" << u << "\t" << time << "\t" << npat[i] << "\t" << npos[i] << "\t" << nevpos[i] << "\t" << ntot[i] << "\n";
				break;

			case EventCoding::admission: 
				cout << f << "\t" << u << "\t" << time << "\t" << npat[i] << "\t" << npos[i] << "\t" << nevpos[i] << "\t" << ntot[i] << "\n";
				ntot[i]++;
				npat[i]++;
				if (evpostest->get(pat) != 0)
				{
					nevpos[i]++;
				}
				cout << f << "\t" << u << "\t" << time << "\t" << npat[i] << "\t" << npos[i] << "\t" << nevpos[i] << "\t" << ntot[i] << "\n";
				break;
	
			case EventCoding::discharge: 
				cout << f << "\t" << u << "\t" << time << "\t" << npat[i] << "\t" << npos[i] << "\t" << nevpos[i] << "\t" << ntot[i] << "\n";
				npat[i]--;
				if (postest->get(pat) != 0)
				{
					postest->remove(pat);
					npos[i]--;	
				}
				if (evpostest->get(pat) != 0)
				{
					nevpos[i]--;
				}
				cout << f << "\t" << u << "\t" << time << "\t" << npat[i] << "\t" << npos[i] << "\t" << nevpos[i] << "\t" << ntot[i] << "\n";
				break;
	
			case EventCoding::possurvtest: 
			case EventCoding::posclintest: 
				cout << f << "\t" << u << "\t" << time << "\t" << npat[i] << "\t" << npos[i] << "\t" << nevpos[i] << "\t" << ntot[i] << "\n";
				if (postest->get(pat) == 0)
				{
					postest->put(pat,pat);
					npos[i]++;
				}
				if (evpostest->get(pat) == 0)
				{
					evpostest->put(pat,pat);
					nevpos[i]++;
				}
				cout << f << "\t" << u << "\t" << time << "\t" << npat[i] << "\t" << npos[i] << "\t" << nevpos[i] << "\t" << ntot[i] << "\n";
				break;

			case EventCoding::negsurvtest: 
			case EventCoding::negclintest: 
			case EventCoding::start:
			case EventCoding::stop:
			default:
				break;
			}
		}
	
		delete data;
		delete hist;
	}
	catch (exception *ex)
	{
		cerr << "Exception:\n\n";
		cerr << ex->what();
	}
}
