#include <stdio.h>
#include <iostream>

#include "infect.h"

int main(int argc, char *argv[])
{
	try
	{
		stringstream errstream (stringstream::out);
		RawEventList **rel = RawEventList::read(cin,1000,errstream);


		int nmax = 11;
		int *counts = new int[nmax];
		
		for (int i=0; i<1000; i++)
		{
			if (rel[i] == 0)
				continue;
			
			
			EventStack *h = new EventStack();
			IntMap *map = rel[i]->getPatientMap();
			rel[i]->getEpisodes(h,map,errstream);

			for (map->init(); map->hasNext(); )
			{
				Patient *p = (Patient *) map->nextValue();
				int eps = 0;
				for (Episode *e = p->firstEpisode(); e != 0; e = e->getNext())
					eps++;
				counts[eps] += 1;
			}
		}

		for (int i=0; i<nmax; i++)
			cout << i << "\t" << counts[i] << "\n";
	}
	catch (exception *e)
	{
		cerr << "Exception:\n\n";
		cerr << e->what();
	}
}
