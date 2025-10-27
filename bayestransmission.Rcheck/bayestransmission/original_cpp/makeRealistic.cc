
#include <stdio.h>
#include <iostream>

#include "infect.h"
#include "modeling.h"

int main(int argc, char *argv[])
{
	try
	{
		Model *mod = new BasicModel(2,0,1);
		stringstream errstream (stringstream::out);
		System *data = new System(cin,errstream);
		SystemHistory *hist = new SystemHistory(data,mod);
		Sampler *s = new Sampler(hist,mod,0);
		s->initializeEpisodes();

		hist->write(cout,0);

		delete s;
		delete hist;
		delete data;
		delete mod;
	}
	catch (exception *ex)
	{
		cerr << "Exception:\n\n";
		cerr << ex->what();
	}
}
