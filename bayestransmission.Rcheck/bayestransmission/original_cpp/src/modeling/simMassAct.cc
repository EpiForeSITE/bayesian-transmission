
#include <stdio.h>
#include <iostream>

#include "modeling.h"

int main(int argc, char *argv[])
{
	try
	{
		int verbose = 0;
		Random *random = new Random(1);

	// Define model.

		MassActionModel *model = new MassActionModel(2,10,0,0,1,0);
		model->getInsituParams()->set(0.9,0,0.1);

		model->getSurveilenceTestParams()->set(0,0,0,0,0);
		model->getSurveilenceTestParams()->set(1,0,0,0,0);
		model->getSurveilenceTestParams()->set(2,0.8,0,0,0);

		model->getOutColParams()->set(0,0.001,0,0,0);
		model->getOutColParams()->set(1,0,0,0,0);
		model->getOutColParams()->set(2,0.01,0,0,0);
		
		((MassActionICP *)model->getInColParams())->set(0,0.01,0,0,0);
		((MassActionICP *)model->getInColParams())->set(1,0,0,0,0);
		((MassActionICP *)model->getInColParams())->set(2,0.01,0,0,0);

		cerr << "Simulating from model:\n";
		cerr << model->header() << "\n";
		cerr << model << "\n";

	// Read event data.

		stringstream errstream (stringstream::out);
		System *data = new System(cin,errstream);
		if (verbose && errstream.str() != "")
			cerr << errstream.str() << "\n";

	// Create state history.

		SystemHistory *hist = new SystemHistory(data,model);
		model->forwardSimulate(hist,random);
		hist->write(cout,0);

	// Clean up.

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
