#include <stdio.h>
#include <iostream>

#include "infect.h"

int main(int argc, char *argv[])
{
	try
	{
	// Initialize parameters and read command line options.

  		int nsims = 1000;
		int seed = 1;
		int verbose = 0;
		int max = 0;
		double param[] = {0.05, 0.95, -6, 0, 0.01, 0, 0.005};

		switch(argc)
		{
		case 4: sscanf(argv[3],"%d",&verbose);
		case 3: sscanf(argv[2],"%d",&seed);
		case 2: sscanf(argv[1],"%d",&nsims);
		}

	// Create and initialize random number generator.

		Random *r = new Random(seed);

	// Read in data and construct event history.

		stringstream errstream (stringstream::out);

		RawEventList *rel = RawEventList::read(cin,errstream);

		History *h = new History(rel,errstream);
		double midtime = 16815; //(h->lastEvent()->getTime() + h->firstEvent()->getTime() ) /2.0;

		if (verbose && errstream.str() != "")
		{
			cerr << errstream.str() << "\n";
		}

	// Create initial model and likelihood calculator.

		Model *m = new Model(param);
		m->setInterceptTime(16775);

	// Create Metropolis sampler for updaing history.

		EpisodeSampler *es = new EpisodeSampler(h,m,r); 

	// Create Gibbs and Metropolis samplers for updating parameter values.

		GibbsParameterSampler *gps = new GibbsParameterSampler(h,m,r); 
		MetroParameterSampler *mps = new MetroParameterSampler(h,m,r); 

	// Run sampler and output parameter values.

  		for (int i=0; i<nsims; i++)
  		{
			es->sampleEpisodes(max,m->getImportation(),m->instCol(midtime),m->getDecolonization());

			gps->sampleImportation(max,1.0,1.0);
			gps->sampleDecolonization(max, 1.0, 0.0);
			gps->sampleFalseNeg(max, 200.0, 800.0);

			mps->sampleColonization(max, 100, 0, 100000.0, 0, 0.01);
			//gps->sampleColonization(max,0,1);

			if (i > nsims/2)
			{
			for (EpisodeEvent *e = h->firstEvent(); e != h->lastEvent(); e = e->getNext())
			{
				if (e->getType()  == colonization)
				{
					cout << e->getTime() << "\t" << e->getPrev()->getTotal() << "\t" << e->getPrev()->getInfected() << "\n";
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
