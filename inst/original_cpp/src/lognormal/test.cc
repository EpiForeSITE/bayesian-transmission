#include <stdio.h>
#include <iostream>

#include "lognormal.h"

int main(int argc, char *argv[])
{
	try
	{
		LogNormalAbxICP *a = new LogNormalAbxICP(2,0,10);
		delete a;
		LogNormalModel *c = new LogNormalModel(2,1,10);
		delete c;
	}
	catch (exception *ex)
	{
		cerr << "Exception:\n\n";
		cerr << ex->what();
	}
}
