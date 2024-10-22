#include <stdio.h>
#include <iostream>

#include "infect.h"

int main(int argc, char *argv[])
{
	try
	{
	// Initialize parameters and read command line options.

		int verbose = 0;

		switch(argc)
		{
		case 2: sscanf(argv[1],"%d",&verbose);
		}

	// Read in data and construct event history.

		stringstream errstream (stringstream::out);
		RawEventList *rel = RawEventList::read(cin,errstream);
		History *h = new History(rel,errstream,0);
		if (verbose && errstream.str() != "")
		{
			cerr << errstream.str() << "\n";
		}

	// Initialize counts.

		int insit = 0;
		int npat = 0;
		int neps = 0;
		int maxpat = 0;
		int minpat = 100000;
		int postests = 0;
		int negtests = 0;
		double starttime = 0;
		double stoptime = 0;
		double patdays = 0;

		for (EpisodeEvent *e = h->firstEvent(); e != 0; e = e->getNext())
		{

			switch(e->getType())
			{
			case start:
				starttime = e->getTime();
				break;

			case stop:
				stoptime = e->getTime();
				break;

			case insitu:
				insit++;
			case admission:
				if (maxpat < e->getTotal())
					maxpat = e->getTotal();
				neps++;
				if (e->getEpisode()->getPrev() == 0)
					npat++;
				break;

			case discharge:
				patdays += e->getTime() - e->getEpisode()->getAdmission()->getTime();

				if (e->getTime() +1 < h->lastEvent()->getTime())
					if (minpat > e->getTotal())
						minpat = e->getTotal();
				break;

			case postest:
				postests++;
				break;

			case negtest:
				negtests++;
				break;

			default:
				cerr << e << "\t" << e->toString(e->getType()) << "\n";
				break;
			}
		}

	// Output the summary.
		
		cout << "Start time:         \t" << starttime << "\n";
		cout << "Stop time:          \t" << stoptime << "\n";
		cout << "Duration:           \t" << stoptime -starttime << "\n";

		cout << "Number of patients: \t" << npat << "\n";
		cout << "Initial inpatients: \t" << insit << "\n";
		cout << "Maximum inpatients: \t" << maxpat << "\n";
		cout << "Minimum inpatients: \t" << minpat << "\n";
		cout << "Mean inpatients:    \t" << patdays/(stoptime-starttime) << "\n";

		cout << "Number of episodes: \t" << neps << "\n";
		cout << "Total patient days: \t" << patdays << "\n";

		cout << "Positive tests:     \t" << postests << "\n";
		cout << "Negative tests:     \t" << negtests << "\n";
	}
	catch (exception *ex)
	{
		cerr << "Exception:\n\n";
		cerr << ex->what();
	}
}
