#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>

#include "lognormal.h"

int main(int argc, char *argv[])
{
	try
	{
	// Set options from command line.

		int verbose = 1;
		int nmetro = 10;
		int seed = 1;
		int forward = 1;
		int cheat = 1;

		ifstream modfile;

		switch(argc)
		{
		case 3: sscanf(argv[2],"%d",&verbose);
		case 2: modfile.open(argv[1]);
			break;
		default:
			cerr << "Usage: testHistLinkLL modelfile [verbose|1]\n";
			exit(1);
		}

	// Make random number generator.

		if (verbose) 
			cerr << "Setting random seed to " << seed << ".\n";

		Random *random = new Random(seed);

	// Read event data.

		if (verbose)
			cerr << "Reading data from stdin.\n";

		stringstream errstream (stringstream::out);

		RawEventList *rel = new RawEventList(cin,errstream);

		if (errstream.str() != "")
		{
			cerr << errstream.str() << "\n";
			cerr << "Exiting due to data format errors.\n";
			exit(1);
		}

		errstream.str("");

		System *data = new System(rel,errstream);

		if (verbose > 1 && errstream.str() != "")
		{
			cerr << "Data corrections made.\n";
			cerr << errstream.str() << "\n";
		}

	// Read model from model specification file.

		if (verbose) 
			cerr << "Reading model from " << argv[1] << ".\n";

		if (!modfile)
		{
			cerr << "Cannot open model input file " << argv[1] << ".\n";
			exit(1);
		}

		List *l = data->getUnits();
		LogNormalModel *model = readModel(modfile,l,nmetro,forward,cheat);

		if (verbose)
		{
			cerr << "Model read successfully.\n";
		}

	// Create state history.

		if (verbose)
			cerr << "Building history structure.\n";

		SystemHistory *hist = new SystemHistory(data,model,false);
		
		if (verbose)
			cerr << "Calculating individual history link log likelihoods.\n";

		// Output header
		cout << "Unit,Link,EventType,Time,PatientID,LogLik" << std::endl;

		// Loop through all history links
		int unit_idx = 0;
		double total_ll = 0.0;
		int num_links = 0;
		int num_inf = 0;
		
		for (Map *h = hist->getUnitHeads(); h->hasNext(); )
		{
			int link_idx = 0;
			for (HistoryLink *l = (HistoryLink *) h->nextValue(); l != 0; l = l->uNext())
			{
				double ll = model->logLikelihood(l);
				Event *e = l->getEvent();
				
				cout << unit_idx << ","
				     << link_idx << ","
				     << e->getType() << ","
				     << std::setprecision(10) << e->getTime() << ",";
				
				if (e->getPatient() != 0) {
					cout << e->getPatient()->getId();
				} else {
					cout << "NULL";
				}
				
				cout << "," << std::setprecision(10) << ll << std::endl;
				
				total_ll += ll;
				num_links++;
				if (std::isinf(ll) && ll < 0) {
					num_inf++;
				}
				
				link_idx++;
			}
			unit_idx++;
		}

		// Calculate overall log likelihood for comparison
		double overall_ll = model->logLikelihood(hist);

		// Output summary to stderr
		if (verbose)
		{
			cerr << "\n=== Summary ===\n";
			cerr << "Total links: " << num_links << "\n";
			cerr << "Links with -Inf: " << num_inf << "\n";
			cerr << "Sum of link log likelihoods: " << total_ll << "\n";
			cerr << "Overall log likelihood: " << overall_ll << "\n";
			cerr << "Difference: " << (overall_ll - total_ll) << "\n";
		}

		delete l;
		delete rel;
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
