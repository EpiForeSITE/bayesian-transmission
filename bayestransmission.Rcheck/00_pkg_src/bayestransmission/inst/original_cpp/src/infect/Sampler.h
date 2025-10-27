
class Sampler : public Object
{
private:

	SystemHistory *hist;
	Model *model;
	Random *rand;

public:

	Sampler(SystemHistory *h, Model *m, Random *r)
	{
		hist = h;
		model = m;
		rand = r;
		initializeEpisodes();
	}

	virtual inline void sampleModel()
	{
		sampleModel(0);
	}

	virtual inline void sampleModel(int max)
	{
		model->update(hist,rand,max);
	}

	virtual inline void sampleEpisodes()
	{
		sampleEpisodes(0);
	}

	virtual void sampleEpisodes(int max)
	{
		model->sampleEpisodes(hist,max,rand);
	}

	void initializeEpisodes()
	{
		Map *pos = hist->positives();

		for (Map *e = hist->getEpisodes(); e->hasNext(); )
		{
			EpisodeHistory *eh = (EpisodeHistory *) e->nextValue();
			Patient  *ppp = eh->admissionLink()->getEvent()->getPatient();
			model->initEpisodeHistory(eh,pos->got(ppp));
		}


		if (model->isCheating())
		{
			for (Map *e = hist->getEpisodes(); e->hasNext(); )
			{
				EpisodeHistory *eh = (EpisodeHistory *) e->nextValue();
				eh->unapply();
			}
	
                	for (HistoryLink *l = hist->getSystemHead(); l != 0; l = l->sNext())
                		l->setCopyApply();
	
			for (Map *e = hist->getEpisodes(); e->hasNext(); )
			{
				EpisodeHistory *eh = (EpisodeHistory *) e->nextValue();
				eh->apply();
			}
		}

		delete pos;
	}
};
