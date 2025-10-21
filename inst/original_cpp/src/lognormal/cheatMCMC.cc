#include <stdio.h>
#include <iostream>
#include <fstream>

#include "lognormal.h"

int main(int argc, char *argv[])
{
	try
	{
	// Set simulation options from command line.

		int verbose = 0;
		int nburn = 0;
		int nsims = 10;
		int nmetro = 10;
		int seed = 1;
		int forward = 0;
		int cheat = 1;

		ifstream modfile;

		switch(argc)
		{
		case 7: sscanf(argv[6],"%d",&nmetro);
		case 6: sscanf(argv[5],"%d",&verbose);
		case 5: sscanf(argv[4],"%d",&nsims);
		case 4: sscanf(argv[3],"%d",&nburn);
		case 3: sscanf(argv[2],"%d",&seed);
		case 2: modfile.open(argv[1]);
			break;
		default:
			cerr << "Usage: runMCMC modelfile [seed|1] [nburn|0] [nsims|1000] [verbose|0] [nmetro|10]\n";
			exit(1);
		}

	// Make random number generator.

		if (verbose) 
			cerr << "Setting random seed to " << seed << ".\n";

		Random *random = new Random(seed);

	// Read event data.

		stringstream errstream (stringstream::out);

		System *data = new System(cin,errstream);

		if (verbose && errstream.str() != "")
			cerr << errstream.str() << "\n";

	// Read model from model specification file.

		if (verbose) 
			cerr << "Reading model from  " << argv[1] << ".\n";

		if (!modfile)
		{
			cerr << "Cannot open model input file " << argv[1] << ".\n";
			exit(1);
		}

		LogNormalModel *model = readModel(modfile,data->getUnits(),nmetro,forward,cheat);

	// Set time origin of model.

		LogNormalICP *icp = (LogNormalICP *) model->getInColParams();
		icp->setTimeOrigin((data->endTime()-data->startTime())/2.0);

	// Create state history.

		SystemHistory *hist = new SystemHistory(data,model);

	// Make and runsampler.

		Sampler *mc = new Sampler(hist,model,random);

		for (int i=0; i<nburn; i++)
		{
			mc->sampleModel();
		}

		cout << model->header() << "\t" << "LogLike" << "\n";
		cout << model << "\t"<< model->logLikelihood(hist) << "\n";
		cout.flush();

		for (int i=0; i<nsims; i++)
		{
			mc->sampleModel();
			cout << model << "\t"<< model->logLikelihood(hist) << "\n";
			cout.flush();
		}

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
