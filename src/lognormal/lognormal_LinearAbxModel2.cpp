#include "lognormal/lognormal.h"

namespace lognormal{

LinearAbxModel2::LinearAbxModel2(int nst, int nmetro, int fw, int ch) : LogNormalModel(nst,nmetro,fw,ch)
{
    // Constructor implementation for LinearAbxModel2
    // Similar to LinearAbxModel but potentially with different ICP setup
    InColParams *icp = getInColParams();
    delete icp;
    icp = new LinearAbxICP(nst,nmetro);
    setInColParams(icp);
}

} // namespace lognormal
