#ifndef ALUN_MODELING_MASSACTIONMODEL_H
#define ALUN_MODELING_MASSACTIONMODEL_H

#include "BasicModel.h"
#include "MassActionICP.h"
#include "RandomTestParams.h"

class MassActionModel : public BasicModel
{
public:

	MassActionModel(int nst, int nmetro, int isdens, int clin, int fw = 0, int ch = 0) : BasicModel(nst,fw,ch)
	{
		isp = new InsituParams(nstates);
		ocp = new OutColParams(nstates,nmetro);
		icp = new MassActionICP(nstates,isdens);
		survtsp = new TestParams(nstates);
		clintsp = ( clin ? new RandomTestParams(nstates) : survtsp ) ;
	}

	~MassActionModel()
	{
		delete ocp;
		delete isp;
		if (clintsp != 0 && clintsp != survtsp)
			delete clintsp;
		delete survtsp;
		delete icp;
	}
};
#endif // ALUN_MODELING_MASSACTIONMODEL_H
