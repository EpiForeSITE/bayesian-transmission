
#include <stdio.h>
#include <iostream>

#include "infect.h"

int main(int argc, char *argv[])
{
	try
	{
		stringstream errstream (stringstream::out);

		System *data = new System(cin,errstream);

		if (errstream.str() != "") 
			cout << errstream.str() << "\n";

		delete data;
	}
	catch (exception *ex)
	{
		cerr << "Exception:\n\n";
		cerr << ex->what();
	}
}
