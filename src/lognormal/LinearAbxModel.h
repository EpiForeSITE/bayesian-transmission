#ifndef ALUN_LOGNORMAL_LINEARABXMODEL_H
#define ALUN_LOGNORMAL_LINEARABXMODEL_H

#include "LogNormalModel.h"
#include "LinearAbxICP.h"

class LinearAbxModel : public LogNormalModel
{
public:

LinearAbxModel(int nst, int nmetro, int abxmode = 0, bool abxtest = true, bool fw = true, bool ch = false);
inline LinearAbxICP *getInColParams() const
{
    return (LinearAbxICP *)icp;
}


};




#endif // ALUN_LOGNORMAL_LINEARABXMODEL_H
