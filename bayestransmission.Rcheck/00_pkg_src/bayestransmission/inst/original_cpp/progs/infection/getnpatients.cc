#include <stdio.h>
#include <iostream>

#include "infect.h"

int main(int argc, char *argv[])
{
	try
	{
		stringstream errstream (stringstream::out);
		RawEventList **rel = RawEventList::read(cin,1000,errstream);
		
		for (int i=0; i<1000; i++)
		{
			if (rel[i] != 0)
			{
				History *h = new History(rel[i],errstream);

				for (EpisodeEvent *e = h->firstEvent(); e != h->lastEvent(); e = e->getNext())
				{
					switch(e->getType())
					{
					case admission:
					case discharge:
					case start:
					case stop:
					case marker:
						cout << i << "\t" 
							<< e->getTime() << "\t" << e->getTotal() << "\n";
					default:
						continue;
					}
				}
  			}
		}
	}
	catch (exception *e)
	{
		cerr << "Exception:\n\n";
		cerr << e->what();
	}
}
