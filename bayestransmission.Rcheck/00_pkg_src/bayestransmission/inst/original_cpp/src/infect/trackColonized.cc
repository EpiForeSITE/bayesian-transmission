
#include <stdio.h>
#include <iostream>

#include "infect.h"

int main(int argc, char *argv[])
{
	try
	{
		int n = 2;

		Model *mod = new UnitTrackingModel(n);
		Map *pos = new Map();

		stringstream errstream (stringstream::out);
		System *data = new System(cin,errstream);

		SystemHistory *hist = new SystemHistory(data,mod);

		for (HistoryLink *h = hist->getSystemHead(); h != 0; h = h->sNext())
		{
			LocationState *u = h->getUState();
			Event *e = h->getEvent();

			if (u != 0)
			{
				Patient *p = e->getPatient();

				int rec = 0;

				switch(e->getType())
				{
				case EventCoding::acquisition:
					if (n == 2)
						rec = 1;
					break;

				case EventCoding::progression:
					if (n == 3)
						rec = 1;
					break;

				case EventCoding::insitu2:
				case EventCoding::admission2:
					rec = 2;
					break;

				case EventCoding::postest:
				case EventCoding::possurvtest:
				case EventCoding::posclintest:
					if (!pos->got(p))
					{
						rec = 3;
						pos->add(p);
					}
					break;

				default:
					break;
				}
				
				
			//	cout << e << " :\t";

				cout << u->getOwner() << "\t" << e->getTime() << "\t" << rec;
				cout << "\t" << u->getColonized() << "\t" << u->getTotal();
				cout << "\n";
			}
			else
			{
			//	cout << e << " :\t";
			//	cout << "\n";
			}
			
		}
	
		delete hist;
		delete mod;
		delete data;
	}
	catch (exception *ex)
	{
		cerr << "Exception:\n\n";
		cerr << ex->what();
	}
}
