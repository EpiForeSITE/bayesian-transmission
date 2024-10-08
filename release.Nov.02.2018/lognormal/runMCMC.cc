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
		int cheat = 0;
		int outputfinal = 0;
		int outputparam = 1;

		ifstream modfile;

		switch(argc)
		{
		case 9: sscanf(argv[8],"%d",&nmetro);
		case 8: sscanf(argv[7],"%d",&outputparam);
		case 7: sscanf(argv[6],"%d",&outputfinal);
		case 6: sscanf(argv[5],"%d",&verbose);
		case 5: sscanf(argv[4],"%d",&nsims);
		case 4: sscanf(argv[3],"%d",&nburn);
		case 3: sscanf(argv[2],"%d",&seed);
		case 2: modfile.open(argv[1]);
			break;
		default:
			cerr << "Usage: runMCMC modelfile [seed|1] [nburn|0] [nsims|1000] [verbose|0] [outputfinal|0] [outputparam|1] [nmetro|10]\n";
			exit(1);
		}

	// Make random number generator.

		if (verbose) 
			cerr << "Setting random seed to " << seed << ".\n";

		Random *random = new Random(seed);

	// Read raw event data.

		if (verbose) 
			cerr << "Reading data from standard input.\n";

		stringstream errstream (stringstream::out);
		System *data = new System(cin,errstream);
		if (verbose > 1 && errstream.str() != "")
			cerr << errstream.str() << "\n";

	// Read model from model specification file.

		if (verbose) 
			cerr << "Reading model from  " << argv[1] << ".\n";

		if (!modfile)
		{
			cerr << "Cannot open model input file " << argv[1] << ".\n";
			exit(1);
		}

		//LogNormalModel *model = LogNormalModel::readModel(modfile,nmetro,forward,cheat);
		//LogNormalModel *model = LogNormalModel::readModel(modfile,data->getUnits(),nmetro,forward,cheat);
		LogNormalModel *model = MixedModel::readModel(modfile,nmetro,forward,cheat);

	// Set time origin of model.

		LogNormalICP *icp = (LogNormalICP *) model->getInColParams();
		icp->setTimeOrigin((data->endTime()-data->startTime())/2.0);

	// Create state history.

		if (verbose) 
			cerr << "Building history structure.\n";

		SystemHistory *hist = new SystemHistory(data,model);

	// Find tests for posterior prediction and, hence, WAIC estimates.

		if (verbose) 
			cerr << "Finding tests for WAIC.\n";

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

		if (verbose) 
			cerr << "Building sampler.\n";

		Sampler *mc = new Sampler(hist,model,random);

		if (verbose) 
		{
			cerr << "Starting parameters.\n";
			model->writeHeader(cerr);
			cerr << "LogLike";
			cerr << "\n";
			cerr << model << "\t\t"<< model->logLikelihood(hist) << "\n";
		}

		if (verbose) 
			cerr << "Burning " << nburn << ".\n";

		for (int i=0; i<nburn; i++)
		{
			mc->sampleEpisodes();
			mc->sampleModel();
		}

		if (verbose) 
			cerr << "Sampling " << nsims << ".\n";

		model->writeHeader(cout);
		cout << "LogLike";
		cout << "\n";
		for (int i=0; i<nsims; i++)
		{
			mc->sampleEpisodes();
			mc->sampleModel();
			if (outputparam)
			{
				cout << model << "\t\t" << model->logLikelihood(hist) << "\n";
				cout.flush();
			}

			for (int j=0; j<wntests; j++)
			{
				HistoryLink *hh = histlink[j];
                        	double p = testtype[j]->eventProb(hh->getPState()->infectionStatus(),hh->getPState()->onAbx(),hh->getEvent()->getType());
				wprob += p;
				wlogprob += log(p);
				wlogsqprob += log(p)*log(p);
			}
		}

		wprob /= wntests * nsims;
		wlogprob /= wntests * nsims;
		wlogsqprob /= wntests * nsims;
		double waic1 = 2*log(wprob) - 4*wlogprob;
		double waic2 = -2 * log(wprob) - 2 * wlogprob*wlogprob + 2 * wlogsqprob;
		cerr << "WAIC 1 2 = \t" << waic1 << "\t" << waic2 << "\n";

		if (outputfinal)
		{
			if (verbose)
				cerr << "Writing complete form of final state.\n";

			for (HistoryLink *l = hist->getSystemHead(); l != 0; l = l->sNext())
			{
				Event *e = l->getEvent();
				switch (e->getType())
				{
				case EventCoding::insitu:
				case EventCoding::insitu0:
				case EventCoding::insitu1:
				case EventCoding::insitu2:
					switch(l->getPState()->infectionStatus())
					{
					case InfectionCoding::uncolonized:
						e->setType(EventCoding::insitu0);
						break;
					case InfectionCoding::latent:
						e->setType(EventCoding::insitu1);
						break;
					case InfectionCoding::colonized:
						e->setType(EventCoding::insitu2);
						break;
					default:
						break;
					}
					break;

				case EventCoding::admission:
				case EventCoding::admission0:
				case EventCoding::admission1:
				case EventCoding::admission2:
					switch(l->getPState()->infectionStatus())
					{
					case InfectionCoding::uncolonized:
						e->setType(EventCoding::admission0);
						break;
					case InfectionCoding::latent:
						e->setType(EventCoding::admission1);
						break;
					case InfectionCoding::colonized:
						e->setType(EventCoding::admission2);
						break;
					default:
						break;
					}
					break;

			
				default:
					break;
				}
			}

			hist->write(cout,5);
		}

		delete [] histlink;
		delete [] testtype;
		delete tests;
		delete mc;
		delete hist;
		delete data;
		delete model;
		delete random;
		delete AbxCoding::sysabx;
	}
	catch (exception *ex)
	{
		cerr << "Exception:\n\n";
		cerr << ex->what();
	}
}
