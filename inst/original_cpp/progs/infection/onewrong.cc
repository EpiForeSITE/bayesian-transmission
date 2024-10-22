#include <stdio.h>
#include <iostream>

#include "infect.h"

int main(int argc, char *argv[])
{
	try
	{
	// Initialize parameters and read command line options.

  		int nsims = 1000;
		int nburn = 500;
		int seed = 1;
		int verbose = 0;
		int max = 0;
		int npar = 8;
		double param[] = {0.00001, 0.05, 0, -6, 0, 0.0, 0, 0.005};

		switch(argc)
		{
		case 5: sscanf(argv[4],"%d",&verbose);
		case 4: sscanf(argv[3],"%d",&nburn);
		case 3: sscanf(argv[2],"%d",&nsims);
		case 2: sscanf(argv[1],"%d",&seed);
		}

	// Create and initialize random number generator.

		Random *r = new Random(seed);

	// Read in data and construct event history.

		stringstream errstream (stringstream::out);

		RawEventList *rel = RawEventList::read(cin,errstream);

		History *h = new History(rel,errstream);
		double midtime = (h->lastEvent()->getTime() + h->firstEvent()->getTime() ) /2.0;

		if (verbose && errstream.str() != "")
		{
			cerr << errstream.str() << "\n";
		}

	// Create initial model and likelihood calculator.

		Model *m = new Model(param);

		//m->setInterceptTime(16775);
		//m->setInterceptTime(0);
		m->setInterceptTime(midtime);

		Likelihood *l = new Likelihood(h,m);

	// Create Metropolis sampler for updaing history.

		EpisodeSampler *es = new EpisodeSampler(h,m,r); 

	// Create Gibbs and Metropolis samplers for updating parameter values.

		GibbsParameterSampler *gps = new GibbsParameterSampler(h,m,r); 
		MetroParameterSampler *mps = new MetroParameterSampler(h,m,r); 

	// Run sampler and output parameter values.

		double *means = new double[npar];
		for (int i=0; i<npar; i++)
			means[i] = 0;

  		for (int i=0; i<nsims; i++)
  		{
			es->sampleEpisodes(max,m->getImportation(),m->instCol(midtime),m->getDecolonization());
		
			gps->sampleProbInsitu(max,1,1);
			gps->sampleImportation(max,1,1);
			gps->sampleColonization(max,1,1);
			gps->sampleFalseNeg(max,1,1);

			m->getParameters(param);
			param[3] = exp(param[3]);

			if (i > nburn)
				for (int j=0; j<npar; j++)
					means[j] += param[j];
  		}

		for (int j=0; j<npar; j++)
			fprintf(stdout," %8.5f\t",means[j]/(nsims-nburn));
		cout << "\n";
	}
	catch (exception *e)
	{
		cerr << "Exception:\n\n";
		cerr << e->what();
	}
}
