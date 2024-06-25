
class ParameterSampler : public Object
{
protected:
	int n;
	Model **m;
	Random *r;

public:
	ParameterSampler(History *h, Model *mod, Random *rand) : Object()
	{
		n = 1;
		m = new Model*[n];
		m[0] = mod;
		r = rand;
	}

	ParameterSampler(Model **mods, int nmods, EpisodeEvent **pts, Random *rand)
	{
		n = nmods;
		m = new Model*[n];
		for (int i=0; i<n; i++)
			m[i] = mods[i];
		r = rand;
	}
};
