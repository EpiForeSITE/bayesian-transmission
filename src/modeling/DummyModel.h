#ifndef ALUN_MODELING_DUMMYMODEL_H
#define ALUN_MODELING_DUMMYMODEL_H

#include "BasicModel.h"

class DummyModel : public BasicModel
{
public:

	DummyModel(int ns) : BasicModel(ns,0,1)
	{
	}

	virtual infect::PatientState *makePatientState(infect::Patient *p)
	{
		return p == 0 ? 0 : new infect::AbxPatientState(p,nstates);
	}

	virtual int needEventType(EventCode e)
	{
		if (cheating)
		{
			switch(e)
			{
			case insitu:
			case insitu0:
			case insitu1:
			case insitu2:
			case admission:
			case admission0:
			case admission1:
			case admission2:
			case discharge:
			case negsurvtest:
			case possurvtest:
			case negclintest:
			case posclintest:
			case acquisition:
			case progression:
			case clearance:
			case abxdose:
			case abxon:
			case abxoff:
				return 1;
			default:
				return 0;
			}
		}
		else
		{
			switch(e)
			{
			case insitu:
			case insitu0:
			case insitu1:
			case insitu2:
			case admission:
			case admission0:
			case admission1:
			case admission2:
			case discharge:
			case negsurvtest:
			case possurvtest:
			case negclintest:
			case posclintest:
			case abxdose:
			case abxon:
			case abxoff:
				return 1;
			default:
				return 0;
			}
		}
	}
};
#endif // ALUN_MODELING_DUMMYMODEL_H
