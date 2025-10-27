
#include <stdio.h>
#include <iostream>

#include "modeling.h"

int main(int argc, char *argv[])
{
	try
	{
		AbxParams *a = new AbxParams(2);
		delete a;
		BasicModel *b = new MassActionModel(2,10,1,0);
		delete b;
		InsituParams *c = new InsituParams(2);
		delete c;
		MassActionICP *d = new MassActionICP(2,0);
		delete d;
		MassActionModel *e = new MassActionModel(2,10,1,0);
		delete e;
		OutColParams *f = new OutColParams(2,10);
		delete f;
		RandomTestParams *g = new RandomTestParams(2);
		delete g;
		TestParamsAbx *h = new TestParamsAbx(2,1);
		delete h;
	}
	catch (exception *ex)
	{
		cerr << "Exception:\n\n";
		cerr << ex->what();
	}
}
