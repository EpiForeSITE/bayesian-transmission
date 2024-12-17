#ifndef ALUN_LOGNORMAL_LINEARABXMODEL_H
#define ALUN_LOGNORMAL_LINEARABXMODEL_H

#include "LogNormalModel.h"

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
};




#endif // ALUN_LOGNORMAL_LINEARABXMODEL_H
