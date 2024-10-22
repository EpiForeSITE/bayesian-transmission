
#include <stdio.h>
#include <iostream>

#include "modeling.h"

int main(int argc, char *argv[])
{
	try
	{
	// Set simulation options from command line.

		Options *o = new Options(argc,argv);

	// Make random number generator.

		Random *random = new Random(o->seed);

	// Define model.

		Model *model = new MassActionModel(o->nstates,o->nmetro,o->densityModel,o->clinical,0,1);

	// Read event data.

		stringstream errstream (stringstream::out);

		System *data = new System(cin,errstream);

		if (o->verbose && errstream.str() != "")
			cerr << errstream.str() << "\n";

	// Create state history.

		SystemHistory *hist = new SystemHistory(data,model);

	// Make and runsampler.

		Sampler *mc = new Sampler(hist,model,random);

		for (int i=0; i<o->nburn; i++)
		{
			mc->sampleModel();
		}

		for (int i=0; i<o->nsims; i++)
		{
			mc->sampleModel();
			cout << model << "\n";
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
