
#include <stdio.h>
#include <iostream>

//#include "models.h"
#include "infect.h"

int main(int argc, char *argv[])
{
	try
	{
		stringstream errstream (stringstream::out);
		System *data = new System(cin,errstream);
		SystemHistory *hist = new SystemHistory(data);

		for (Map *m = hist->getPatientHeads(); m->hasNext(); )
		{
			Patient *p = (Patient *) m->next();
			
			for (HistoryLink *h = (HistoryLink *) m->get(p); h != 0; h = h->pNext())
			{
				switch(h->getEvent()->getType())
				{
				case EventCoding::admission:
				case EventCoding::insitu:
					cout << p << "\t";
					break;

				case EventCoding::discharge: 
					cout << "\n";
					break;

				case EventCoding::possurvtest: 
				case EventCoding::posclintest: 
					cout << "+";
					break;

				case EventCoding::negsurvtest: 
				case EventCoding::negclintest: 
					cout << "-";
					break;

				default:
					break;
				}
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
