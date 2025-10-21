class BasicModel : public UnitLinkedModel
{
public:

	BasicModel(int nst) : UnitLinkedModel(nst,0,0)
	{
	}

	BasicModel(int nst, int fw, int ch) : UnitLinkedModel(nst,fw,ch)
	{
	}


public:

	virtual void forwardSimulate(SystemHistory *h, Random *r)
	{
		ForwardSimulator::forwardSimulate(this,h,r);
	}

	virtual void initEpisodeHistory(EpisodeHistory *eh, bool pos)
	{
		ConstrainedSimulator::initEpisodeHistory(this,eh,pos);
	}

	virtual void sampleEpisodes(SystemHistory *h, int max, Random *rand)
	{
		ConstrainedSimulator::sampleEpisodes(this,h,max,rand);
	}
};
