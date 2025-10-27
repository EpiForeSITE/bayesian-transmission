#include <stdio.h>
#include <iostream>
#include <fstream>
#include <ctime>

#include "ltcf.h"

int main(int argc, char *argv[])
{
	try
	{
		int verbose = 0;
		ifstream modfile;

		switch(argc)
		{
		case 2: modfile.open(argv[1]);
			break;
		default:
			cerr << "Usage: listtest modelfile\n";
			exit(1);
		}


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

		for (IntMap *i = data->getPatients(); i->hasNext(); )
		{
			Patient *pat = (Patient *) i->nextValue();
			cout << pat << "\n";
		}

		delete rel;
		delete data;
		util::sanitize();
	}
	catch (exception *ex)
	{
		cerr << "Exception:\n\n";
		cerr << ex->what();
	}
}
