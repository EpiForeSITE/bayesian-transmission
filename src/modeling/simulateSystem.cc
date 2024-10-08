
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

		Model *model = new MassActionModel(o->nstates,o->nmetro,o->densityModel,o->clinical,1,0);

		cerr << "Simulating from model:\n";
		cerr << model << "\n";

	// Read event data.

		stringstream errstream (stringstream::out);

		System *data = new System(cin,errstream);

		if (o->verbose && errstream.str() != "")
			cerr << errstream.str() << "\n";

	// Create state history.

		SystemHistory *hist = new SystemHistory(data,model);

		model->forwardSimulate(hist,random);

		if (o->allout)
		{
			cerr << "Complete output\n";
			hist->write(cout,2);
		}
		else
		{
			cerr << "Realistic output\n";
			hist->write(cout,0);
		}

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
