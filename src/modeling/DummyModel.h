#ifndef ALUN_MODELING_DUMMYMODEL_H
#define ALUN_MODELING_DUMMYMODEL_H

#include "BasicModel.h"

namespace models{

class DummyModel : public BasicModel
{
public:

	DummyModel(int ns) : BasicModel(ns,0,1){}

	virtual infect::PatientState *makePatientState(infect::Patient *p);
    virtual int needEventType(EventCode e);

};
} // namespace models
#endif // ALUN_MODELING_DUMMYMODEL_H
