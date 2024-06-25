
#include <stdio.h>
#include <iostream>

#include "infect.h"
#include "modeling.h"

int main(int argc, char *argv[])
{
	try
	{
		Model *mod = new DummyModel(2);
		stringstream errstream (stringstream::out);
		System *data = new System(cin,errstream);
		SystemHistory *hist = new SystemHistory(data,mod);
		Sampler *s = new Sampler(hist,mod,0);
		s->initializeEpisodes();

		for (Map *m = hist->getPatientHeads(); m->hasNext(); )
		{
			Patient *p = (Patient *) m->next();
	
			cout << p << "\t";
			
			for (HistoryLink *h = (HistoryLink *) m->get(p); h != 0; h = h->pNext())
			{
				switch(h->getEvent()->getType())
				{
				case EventCoding::insitu:
				case EventCoding::insitu0:
					cout << " [";
					break;

				case EventCoding::insitu1:
				case EventCoding::insitu2:
					cout << "( [";

				case EventCoding::admission:
					cout << " [";
					break;

				case EventCoding::admission0:
					if (h->pPrev() != 0 && h->pPrev()->getPState()->infectionStatus() != h->getPState()->infectionStatus())
						cout << ") [";
					else
						cout << " [";
					break;

				case EventCoding::admission1:
				case EventCoding::admission2:
					if (h->pPrev() == 0)
						cout << "( [";
					else if (h->pPrev()->getPState()->infectionStatus() != h->getPState()->infectionStatus())
						cout << "( [";
					else
						cout << " [";
					break;

				case EventCoding::acquisition:
					cout << "(";
					break;

				case EventCoding::clearance:
					cout << ")";
					break;

				case EventCoding::discharge: 
					cout << "] ";
					break;

				case EventCoding::possurvtest: 
					cout << "+";
					break;

				case EventCoding::posclintest: 
					cout << "!";
					break;

				case EventCoding::negsurvtest: 
					cout << "-";
					break;

				case EventCoding::negclintest: 
					cout << ".";
					break;

				case EventCoding::abxon:
					if (((AbxPatientState*)h->getPState())->onAbx() == 1)
						cout << "<";
					break;

				case EventCoding::abxoff:
					if (((AbxPatientState*)h->getPState())->onAbx() == 0)
						cout << ">";
					break;

				case EventCoding::abxdose:
					cout << "x";
					break;

				default:
					break;
				}
			}

			cout << "\n";
		}

		delete s;
		delete hist;
		delete data;
		delete mod;
	}
	catch (exception *ex)
	{
		cerr << "Exception:\n\n";
		cerr << ex->what();
	}
}
