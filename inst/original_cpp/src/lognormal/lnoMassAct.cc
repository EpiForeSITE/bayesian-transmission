#include <stdio.h>
#include <iostream>

#include "lognormal.h"

int main(int argc, char *argv[])
{
	try
	{
		Random *random = new Random(1);
		int verbose = 0;
		int nsims = 1000;

	// Define starting model.

		MassActionModel *model = new MassActionModel(2,10,0,0);

		InsituParams *isp = model->getInsituParams();
		isp->set(0.9,0,0.1);
		isp->setPriors(1,0,1);
		isp->setUpdate(1,0,1);

		TestParams *tsp = model->getSurveilenceTestParams();
		tsp->set(0,0,0,0.5,2);
		tsp->set(1,0,0,0.5,2);
		tsp->set(2,0.8,1,0.5,2);

		OutColParams *ocp = model->getOutColParams();
		ocp->set(0,0.001,1,1,1);
		ocp->set(1,0,0,0,0);
		ocp->set(2,0.01,0,1,1);

	// Read event data.

		stringstream errstream (stringstream::out);
		System *data = new System(cin,errstream);
		if (verbose && errstream.str() != "")
			cerr << errstream.str() << "\n";

	// Create state history.

		SystemHistory *hist = new SystemHistory(data,model);

// Make a log normal Mass Action version.

		InColParams *icp = ((MassActionICP *)model->getInColParams());
		delete icp;

		LNMassActionICP *micp = new LNMassActionICP(2,0);
		micp->setWithLogTransform(0,0,0.01,1,0.01,1);
		micp->setWithLogTransform(1,0,0.01,0,0.0,1);
		micp->setWithLogTransform(2,0,0.01,1,0.01,1);

		model->setInColParams(micp);

	// Make and runsampler.

		Sampler *mc = new Sampler(hist,model,random);

		cout << model->header() << "\t" << "LogLike" << "\n";
		cout << model << "\t" << model->logLikelihood(hist) << "\n";
		cout.flush();

		for (int i=0; i<nsims; i++)
		{
			mc->sampleEpisodes();
			mc->sampleModel();
			cout << model << "\t" << model->logLikelihood(hist) << "\n";
			cout.flush();
		}

	// Clean up.

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
