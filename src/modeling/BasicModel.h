#ifndef ALUN_MODELING_BASICMODEL_H
#define ALUN_MODELING_BASICMODEL_H

#include "UnitLinkedModel.h"
#include "ForwardSimulator.h"
#include "ConstrainedSimulator.h"

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

	virtual void forwardSimulate(infect::SystemHistory *h, Random *r)
	{
		ForwardSimulator::forwardSimulate(this,h,r);
	}

	virtual void initEpisodeHistory(infect::EpisodeHistory *eh, bool pos)
	{
		ConstrainedSimulator::initEpisodeHistory(this,eh,pos);
	}

	virtual void sampleEpisodes(infect::SystemHistory *h, int max, Random *rand)
	{
		ConstrainedSimulator::sampleEpisodes(this,h,max,rand);
	}

};
#endif // ALUN_MODELING_BASICMODEL_H
