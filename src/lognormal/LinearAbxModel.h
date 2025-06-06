#ifndef ALUN_LOGNORMAL_LINEARABXMODEL_H
#define ALUN_LOGNORMAL_LINEARABXMODEL_H

#include "LogNormalModel.h"
#include "LinearAbxICP.h"

class LinearAbxModel : public LogNormalModel
{
public:

LinearAbxModel(int nst, int nmetro, int fw, int ch) : LogNormalModel(nst,nmetro,fw,ch)
{
    InColParams *icp = getInColParams();
    delete icp;
    icp = new LinearAbxICP(nst,nmetro);
    setInColParams(icp);
}
inline LinearAbxICP *getInColParams() const
{
    return (LinearAbxICP *)icp;
}


};




#endif // ALUN_LOGNORMAL_LINEARABXMODEL_H
