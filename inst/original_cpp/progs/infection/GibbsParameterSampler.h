
class GibbsParameterSampler : public ParameterSampler 
{
private:
	Counter *c;

public:
	GibbsParameterSampler(History *h, Model *mod, Random *rand) : ParameterSampler(h,mod,rand)
	{
		c = new Counter(h->firstEvent(),h->lastEvent());
	}

	GibbsParameterSampler(Model **mods, int nmods, EpisodeEvent **pts, Random *rand) : ParameterSampler(mods,nmods,pts,rand)
	{
		c = new Counter(nmods,pts);
	}

	Counter *getCounter()
	{
		return c;
	}

	void sampleProbInsitu(int max, double aa, double bb)
	{
		double a = aa;
		double b = bb;

		c->countInsituStats(&a,&b);
	
		double res = max ? (a-1) / (a-1 + b-1) : r->rbeta(a,b);

		for (int i=0; i<n; i++)
			m[i]->setProbInsitu(res);
	}

	void sampleImportation(int max, double aa, double bb)
	{
		double a = aa;
		double b = bb;

		c->countImportationStats(&a,&b);

		double res = max ? (a-1) / (a-1 + b-1) : r->rbeta(a,b);

		for (int i=0; i<n; i++)
			m[i]->setImportation(res);
	}

	void sampleFalseNeg(int max, double aa, double bb)
	{
		double a = aa;
		double b = bb;

		c->countFalseNegStats(&a,&b);

		double res = max ? (a-1) / (a-1 + b-1) : r->rbeta(a,b);

		for (int i=0; i<n; i++)
			m[i]->setFalseNeg(res);
	}

	void sampleFalsePos(int max, double aa, double bb)
	{
		double a = aa;
		double b = bb;

		c->countFalsePosStats(&a,&b);

		double res = max ? (a-1) / (a-1 + b-1) : r->rbeta(a,b);

		for (int i=0; i<n; i++)
			m[i]->setFalsePos(res);
	}

	void sampleColonization(int max, double a, double b)
	{
		double shape = a;
		double rate = b;

		c->countColonizationStats(&shape,&rate);

		if (shape >= 1 && rate > 0)
		{
			double res = max ? (shape-1)/rate : r->rgamma(shape,rate);
			for (int i=0; i<n; i++)
				m[i]->setColonization(res);
		}
	}

	void sampleDecolonization(int max, double a, double b)
	{
		double shape = a;
		double rate = b;

		c->countDecolonizationStats(&shape,&rate);

		if (shape >= 1 && rate > 0)
		{
			double res = max ? (shape-1)/rate : r->rgamma(shape,rate);
			for (int i=0; i<n; i++)
				m[i]->setDecolonization(res);
		}
	}

	void sampleProbInsitu(int i, int max, double aa, double bb)
	{
		double a = aa;
		double b = bb;
		c->countInsituStats(i,&a,&b);
		double res = max ? (a-1) / (a-1 + b-1) : r->rbeta(a,b);
		m[i]->setProbInsitu(res);
	}

	void sampleImportation(int i, int max, double aa, double bb)
	{
		double a = aa;
		double b = bb;
		c->countImportationStats(i,&a,&b);
		double res = max ? (a-1) / (a-1 + b-1) : r->rbeta(a,b);
		m[i]->setImportation(res);
	}

	void sampleFalseNeg(int i, int max, double aa, double bb)
	{
		double a = aa;
		double b = bb;
		c->countFalseNegStats(i,&a,&b);
		double res = max ? (a-1) / (a-1 + b-1) : r->rbeta(a,b);
		m[i]->setFalseNeg(res);
	}

	void sampleFalsePos(int i, int max, double aa, double bb)
	{
		double a = aa;
		double b = bb;
		c->countFalsePosStats(i,&a,&b);
		double res = max ? (a-1) / (a-1 + b-1) : r->rbeta(a,b);
		m[i]->setFalsePos(res);
	}

	void sampleColonization(int i, int max, double a, double b)
	{
		double shape = a;
		double rate = b;
		c->countColonizationStats(i,&shape,&rate);
		if (shape >= 1 && rate > 0)
		{
			double res = max ? (shape-1)/rate : r->rgamma(shape,rate);
			m[i]->setColonization(res);
		}
	}

	void sampleDecolonization(int i, int max, double a, double b)
	{
		double shape = a;
		double rate = b;
		c->countDecolonizationStats(i,&shape,&rate);
		if (shape >= 1 && rate > 0)
		{
			double res = max ? (shape-1)/rate : r->rgamma(shape,rate);
			m[i]->setDecolonization(res);
		}
	}
};
