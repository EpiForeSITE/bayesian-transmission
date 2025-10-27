
#include <stdio.h>
#include <iostream>

#include "modeling.h"

int main(int argc, char *argv[])
{
	try
	{
	// Set simulation options from command line.

		Options *o = new Options (argc,argv);
		o->verbose = 1;

	// Define model.

		Model *model = new MassActionModel(o->nstates,o->nmetro,o->densityModel,o->clinical,0,0);

	// Read event data.

		stringstream errstream (stringstream::out);

		System *data = new System(cin,errstream);

		if (o->verbose && errstream.str() != "")
			//cerr << errstream.str() << "\n";
			cout << errstream.str() << "\n";

	// Create state history.


		SystemHistory *hist = new SystemHistory(data,model);

		hist->write(cout,0);

		delete o;
		delete hist;
		delete data;
		delete model;
	}
	catch (exception *ex)
	{
		cerr << "Exception:\n\n";
		cerr << ex->what();
	}
}
