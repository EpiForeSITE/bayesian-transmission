#include "lognormal/lognormal.h"

namespace lognormal{

LinearAbxModel::LinearAbxModel(int nst, int nmetro, int abxmode, bool abxtest, bool fw, bool ch) :
    LogNormalModel(nst, nmetro, abxmode, abxtest, fw, ch)
{
    InColParams *icp = getInColParams();
    delete icp;
    icp = new LinearAbxICP(nst,nmetro);
    setInColParams(icp);
}

} // namespace lognormal
