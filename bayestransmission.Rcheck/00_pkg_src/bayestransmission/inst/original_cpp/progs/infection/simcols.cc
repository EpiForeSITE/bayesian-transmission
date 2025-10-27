#include <stdio.h>
#include <iostream>

#include "infect.h"

int main(int argc, char *argv[])
{
	try
	{
	// Initialize parameters and read command line options.

		int seed = 1;
		int verbose = 1;
		int npar = 8;
		double param[] = {0.15, 0.15, 0.05, log(0.0025), 0, 0.020, 0, 0.35};

		switch(argc)
		{
		case 3: sscanf(argv[2],"%d",&verbose);
		case 2: sscanf(argv[1],"%d",&seed);
		}

	// Create and initialize random number generator.

		Random *r = new Random(seed);

	// Create initial model.

		Model *m = new Model(param);

	// Read in data and construct event history.

		stringstream errstream (stringstream::out);
		RawEventList *rel = RawEventList::read(cin,errstream);
		History *h = new History(rel,errstream,0);
		if (verbose && errstream.str() != "")
		{
			cerr << errstream.str() << "\n";
		}

	// Initialize colonized and susceptible sets.
	
		List *Col = new List();
		List *Sus = new List();

	// Iterate over events randomizing test results.

		EventCode type = start;
		bool output = 0;
		double imp = 0;

		for (EpisodeEvent *e = h->firstEvent(); e != 0; )
		{

		// Update Col and Sus given the event. Set type and output option.

			switch(e->getType())
			{
			case insitu:
				if (r->runif() < m->getProbInsitu())
					Col->append(e->getEpisode());
				else
					Sus->append(e->getEpisode());

				type = admission;
				output = 1;
				break;

			case admission:
				if (e->getEpisode()->getPrev() == 0)
				{
					imp = m->getImportation();
				}
				else
				{
					int ps = e->getEpisode()->getPrev()->getDischarge()->getStatus();
					double pt = e->getEpisode()->getPrev()->getDischarge()->getTime();
					imp = m->getImportation(ps,e->getTime()-pt);
				}
					
				if (r->runif() < imp)
					Col->append(e->getEpisode());
				else
					Sus->append(e->getEpisode());

				type = admission;
				output = 1;
				break;

			case discharge:
				if (Col->contains(e->getEpisode()))
				{
					Col->remove(e->getEpisode());
					e->setStatus(1);
				}

				if (Sus->contains(e->getEpisode()))
				{
					Sus->remove(e->getEpisode());
					e->setStatus(0);
				}

				type = discharge;
				output = 1;
				break;

			case postest:
			case negtest:
				if (Col->contains(e->getEpisode()))
				{
					type = ( r->runif() < m->getFalseNeg() ? negtest : postest);
				}

				if (Sus->contains(e->getEpisode()))
				{
					type = ( r->runif() < m->getFalsePos() ? postest : negtest);
				}

				output = 1;
				break;

			case colonization:
				Sus->remove(e->getEpisode());
				Col->append(e->getEpisode());
				output = 0;
				break;

			case decolonization:
				Col->remove(e->getEpisode());
				Sus->append(e->getEpisode());
				output = 0;
				break;

			default:
				output = 0;
			//	cerr << e << "\t" << e->toString(e->getType()) << "\n";
				break;
			}

		// Output the new event information.

			if (output)
			{
				cout << e->getTime() << "\t" ;
				cout << e->getEpisode()->getPatient()->id() << "\t" ;
				cout << e->decode(type) << "\n";
			}

		// Set the next event.

			double rcol = Col->size() * Sus->size() * m->getColonization();
			double rdec = Col->size() * m->getDecolonization();
			double dt = e->getTime() + r->rexp() / (rcol+rdec);

			if (!e->getNext() || (rcol <=0 && rdec <= 0) || dt > e->getNext()->getTime())
			{
				e = e->getNext();
			}
			else
			{
				if (r->runif() < rcol / (rcol + rdec))
				{
					Episode *ep = (Episode *) Sus->random(r);
					e = new EpisodeEvent(dt,colonization,ep,e->getNext());
				}
				else
				{
					Episode *ep = (Episode *) Col->random(r);
					e = new EpisodeEvent(dt,decolonization,ep,e->getNext());
				}
			}
		}
	}
	catch (exception *ex)
	{
		cerr << "Exception:\n\n";
		cerr << ex->what();
	}
}
