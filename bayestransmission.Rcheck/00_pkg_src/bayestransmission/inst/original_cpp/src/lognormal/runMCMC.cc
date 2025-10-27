#include <stdio.h>
#include <iostream>
#include <fstream>
#include <ctime>

#include "lognormal.h"

int main(int argc, char *argv[])
{
	try
	{
		clock_t start = clock();

	// Set simulation options from command line.

		int max = 0;
		int dotime = 1;
		int verbose = 0;
		int nburn = 0;
		int nsims = 10;
		int nmetro = 10;
		int seed = 2;
		int forward = 0;
		int cheat = 0;
		int outputfinal = 0;
		int outputparam = 1;
		int outputwaic = 1;

		ifstream modfile;

		switch(argc)
		{
		case 10: sscanf(argv[9],"%d",&nmetro);
		case 9: sscanf(argv[8],"%d",&outputwaic);
		case 8: sscanf(argv[7],"%d",&outputparam);
		case 7: sscanf(argv[6],"%d",&outputfinal);
		case 6: sscanf(argv[5],"%d",&verbose);
		case 5: sscanf(argv[4],"%d",&nsims);
		case 4: sscanf(argv[3],"%d",&nburn);
		case 3: sscanf(argv[2],"%d",&seed);
		case 2: modfile.open(argv[1]);
			break;
		default:
			cerr << "Usage: runMCMC modelfile [seed|1] [nburn|0] [nsims|1000] [verbose|0] [outputfinal|0] [outputparam|1] [outputwaic|1] [nmetro|10]\n";
			exit(1);
		}

	// Make random number generator.

		if (verbose) 
			cerr << "Setting random seed to " << seed << ".\n";

		Random *random = new Random(seed);

	// Read raw event data.

		if (verbose) 
			cerr << "Reading data from standard input.\n";

		stringstream errstream(stringstream::out);

		RawEventList *rel = new RawEventList(cin,errstream);

		if (errstream.str() != "")
		{
			cerr << errstream.str() << "\n";
			cerr << "Exiting due to data format errors.\n";
			cerr << "Data should have a line for each event in the format:\n";
			cerr << "\t facility<int> unit<int> time<double> patient<long> type<int> [comment<arbitrary>]\n";
			exit(1);
		}

		errstream.str("");

		System *data = new System(rel,errstream);


		if (verbose > 1 && errstream.str() != "")
		{
			cerr << "Data corrections made.\n";
			cerr << errstream.str();
		}

	// Read model from model specification file.

		if (verbose) 
			cerr << "Reading model from  " << argv[1] << ".\n";

		if (!modfile)
		{
			cerr << "Cannot open model input file " << argv[1] << ".\n";
			exit(1);
		}

		List *l = data->getUnits();
		LogNormalModel *model = readModel(modfile,l,nmetro,forward,cheat);

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
		if (outputwaic)
		{
			for (tests->init(); tests->hasNext(); wntests++)
			{
				histlink[wntests] = (HistoryLink *) tests->next();
	
				if (histlink[wntests]->getEvent()->isClinicalTest())
					testtype[wntests] = model->getClinicalTestParams();
				else
					testtype[wntests] = model->getSurveilenceTestParams();
			}
		}

	// If just need events, output and quit.

		if (seed < 0)
		{
			hist->write(cout);
			exit(0);
		}
		
	// Make and runsampler.

		if (verbose) 
			cerr << "Building sampler.\n";

		Sampler *mc = new Sampler(hist,model,random);

		if (verbose) 
		{
			cerr << "Starting parameters.\n";
			cerr << model->header() << "\t" << "LogLike" << "\n";
			cerr << model << "\t" << model->logLikelihood(hist) << "\n";
		}

		if (verbose) 
			cerr << "Burning " << nburn << ".\n";

		for (int i=0; i<nburn; i++)
		{
			if (!cheat)
				mc->sampleEpisodes();
			mc->sampleModel();
		}

		if (verbose) 
			cerr << "Sampling " << nsims << ".\n";

		if (outputparam)
		{
			cout << model->header() << "\t" << "LogLike" << "\n";
			cout << model << "\t"<< model->logLikelihood(hist) << "\n";
			cout.flush();
		}

		for (int i=0; i<nsims; i++)
		{
			if (!cheat)
				mc->sampleEpisodes();
			mc->sampleModel(max);
			if (outputparam)
			{
				cout << model << "\t" << model->logLikelihood(hist) << "\n";
				cout.flush();
			}

			if (outputwaic)
			{
				for (int j=0; j<wntests; j++)
				{
					HistoryLink *hh = histlink[j];
                        		double p = testtype[j]->eventProb(hh->getPState()->infectionStatus(),hh->getPState()->onAbx(),hh->getEvent()->getType());
					wprob += p;
					wlogprob += log(p);
					wlogsqprob += log(p)*log(p);
				}
			}
		}

		if (outputwaic)
		{
			wprob /= wntests * nsims;
			wlogprob /= wntests * nsims;
			wlogsqprob /= wntests * nsims;
			double waic1 = 2*log(wprob) - 4*wlogprob;
			double waic2 = -2 * log(wprob) - 2 * wlogprob*wlogprob + 2 * wlogsqprob;
			cout << "WAIC 1 2 = \t" << waic1 << "\t" << waic2 << "\n";
		}

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

		if (dotime)
		{
			double time = (clock() - start) / (double) CLOCKS_PER_SEC; 
			cerr << "\t" << hist->sumocc() << "\t" << time << "\n";
		}

		delete l;
		delete rel;
		delete [] histlink;
		delete [] testtype;
		delete tests;
		delete mc;
		delete hist;
		delete data;
		delete model;
		delete random;
		delete AbxCoding::sysabx;
		delete AbxCoding::syseverabx;
		util::sanitize();

	}
	catch (exception *ex)
	{
		cerr << "Exception:\n\n";
		cerr << ex->what();
	}
}
