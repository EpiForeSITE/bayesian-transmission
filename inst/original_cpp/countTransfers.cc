
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

		int *got = 0;
		int *nadmits = 0;
		int *newads = 0;
		int **transf = 0;
		int uid = 0;

		for (Map *m = hist->getFacilityHeads(); m->hasNext(); )
		{
			Facility *f = (Facility *) m->next();
			for (IntMap *im = f->getUnits(); im->hasNext(); )
			{
				int y = im->next();
				if (y > uid)
					uid = y;
			}
		}

		uid = uid+1;
		got = new int[uid];
		nadmits = new int[uid];
		newads = new int[uid];
		transf = new int*[uid];

		for (int i=0; i<uid; i++)
		{
			got[i] = 0;
			nadmits[i] = 0;
			newads[i] = 0;
			transf[i] = new int[uid];
			for (int j=0; j<uid; j++)
				transf[i][j] = 0;
		}

		for (Map *m = hist->getFacilityHeads(); m->hasNext(); )
		{
			Facility *f = (Facility *) m->next();
			for (IntMap *im = f->getUnits(); im->hasNext(); )
				got[im->next()] = 1;
		}

		for (Map *m = hist->getFacilityHeads(); m->hasNext(); )
		{
			Facility *f = (Facility *) m->next();
			int i = 0;
			int j = 0;

			for (IntMap *im = f->getUnits(); im->hasNext(); )
			{
				Unit *u = (Unit *) im->nextValue();

				for (HistoryLink *h = (HistoryLink *) hist->getUnitHeads()->get(u); h != 0; h = h->uNext())
				{
					switch(h->getEvent()->getType())
					{
					case EventCoding::admission: 

						i = h->getEvent()->getUnit()->getId();

						nadmits[i] += 1;

						if (h->pPrev() == 0)
						{
							newads[i] += 1;
						}
						else
						{
							j = h->pPrev()->getEvent()->getUnit()->getId();
							transf[j][i] += 1;
						}

						break;
	
					default:
						break;
					}
				}
			}
		}
	
			
		cout << "Unit\t";
		for (int i=0; i<uid; i++)
			if (got[i])
				cout << i << "\t"; 
		cout << "\n";

		cout << "\n";

		cout << "New\t";
		for (int i=0; i<uid; i++)
			if (got[i])
				cout << newads[i] << "\t"; 
		cout << "\n";
		cout << "\n";
		
		for (int i=0; i<uid; i++)
			if (got[i])
			{
				cout << i << "\t";
				for (int j=0; j<uid; j++)
					if (got[j])
					{
						cout << transf[i][j] << "\t";
					}
				cout << "\n";
			}

		cout << "\n";

		cout << "Total\t";
		for (int i=0; i<uid; i++)
			if (got[i])
				cout << nadmits[i] << "\t"; 
		cout << "\n";

		delete[] got;
		delete [] nadmits;
		delete [] newads;
		for (int i=0; i<uid; i++)
			delete [] transf[i];
		delete [] transf;
		
		delete data;
		delete hist;
	}
	catch (exception *ex)
	{
		cerr << "Exception:\n\n";
		cerr << ex->what();
	}
}
