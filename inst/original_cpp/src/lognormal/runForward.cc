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
		int nmetro = 0;
		int seed = 1;
		int forward = 1;
		int cheat = 0;
		int showall = 0;

		ifstream modfile;

		switch(argc)
		{
		case 5: sscanf(argv[4],"%d",&verbose);
		case 4: sscanf(argv[3],"%d",&seed);
		case 3: sscanf(argv[2],"%d",&showall);
		case 2: modfile.open(argv[1]);
			break;
		default:
			cerr << "Usage: runForward modelfile [showall|0] [seed|1] [verbose|0]\n";
			exit(1);
		}

	// Make random number generator.

		if (verbose) 
			cerr << "Setting random seed to " << seed << ".\n";

		Random *random = new Random(seed);

	// Read event data.

		if (verbose)
			cerr << "Reading data.\n";

		stringstream errstream (stringstream::out);

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
			cerr << errstream.str() << "\n";
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

		if (verbose)
		{
			cerr << "Simulating from model:\n";
			cerr << model << "\n";
		}

	// Create state history.

		SystemHistory *hist = new SystemHistory(data,model);
		
		if (verbose)
			cerr << "Starting simulation.\n";

		model->forwardSimulate(hist,random);

		if (showall)
		{
			if (verbose)
				cerr << "Showing complete simulation\n";
			hist->write(cout,2);
		}
		else
		{
			if (verbose)
				cerr << "Showing observable simulation\n";
			hist->write(cout,0);
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
