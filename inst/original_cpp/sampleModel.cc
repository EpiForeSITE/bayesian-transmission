#include <stdio.h>
#include <iostream>

#include "modeling.h"

int main(int argc, char *argv[])
{
	try
	{
	// Set simulation options from command line.

		Options *o = new Options (argc,argv);

	// Make random number generator.

		Random *random = new Random(o->seed);

	// Define model.

		BasicModel *model = new MassActionModel(o->nstates,o->nmetro,o->densityModel,o->clinical);

	// Read event data.

		stringstream errstream (stringstream::out);

		//System *data = new System(cin,errstream,model);
		System *data = new System(cin,errstream);

		if (o->verbose && errstream.str() != "")
			cerr << errstream.str() << "\n";

	// Create state history.

		SystemHistory *hist = new SystemHistory(data,model);

	// Find tests for posterior prediction and, hence, WAIC estimates.

		List *tests = hist->getTestLinks();
		TestParams **testtype = new TestParams*[tests->size()];
		HistoryLink **histlink = new HistoryLink*[tests->size()];
		
		int wntests = 0;
		double wprob = 0;
		double wlogprob = 0;
		double wlogsqprob = 0;

		for (tests->init(); tests->hasNext(); wntests++)
		{
			histlink[wntests] = (HistoryLink *) tests->next();

			if (histlink[wntests]->getEvent()->isClinicalTest())
				testtype[wntests] = model->getClinicalTestParams();
			else
				testtype[wntests] = model->getSurveilenceTestParams();
		}
		
	// Make and runsampler.

		Sampler *mc = new Sampler(hist,model,random);

		for (int i=0; i<o->nburn; i++)
		{
			mc->sampleEpisodes();
			mc->sampleModel();
		}

		for (int i=0; i<o->nsims; i++)
		{
			mc->sampleEpisodes();
			mc->sampleModel();
			cout << model << "\t";
			cout << model->logLikelihood(hist);
			cout << "\n";
			cout.flush();

			for (int j=0; j<wntests; j++)
			{
				HistoryLink *hh = histlink[j];
                        	double p = testtype[j]->eventProb(hh->getPState()->infectionStatus(),hh->getPState()->onAbx(),hh->getEvent()->getType());
				wprob += p;
				wlogprob += log(p);
				wlogsqprob += log(p)*log(p);
			}
		}

		wprob /= wntests * o->nsims;
		wlogprob /= wntests * o->nsims;
		wlogsqprob /= wntests * o->nsims;
		double waic1 = 2*log(wprob) - 4*wlogprob;
		double waic2 = -2 * log(wprob) - 2 * wlogprob*wlogprob + 2 * wlogsqprob;
		cerr << "WAIC 1 2 = \t" << waic1 << "\t" << waic2 << "\n";

		delete mc;
		delete hist;
		delete data;
		delete model;
		delete random;
	}
	catch (exception *ex)
	{
		cerr << "Exception:\n\n";
		cerr << ex->what();
	}
}
