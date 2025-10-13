#include "lognormal/lognormal.h"

namespace lognormal{

LinearAbxICP::LinearAbxICP(int nst, int nmet, int nacqpar) : LogNormalICP(nst,nacqpar,3,3,nmet)
{
}

string LinearAbxICP::header() const
{
    stringstream s;
    s << "LABX.base";
    s << "\t" << "LABX.time";
    s << "\t" << "LABX.mass.mx";
    s << "\t" << "LABX.freq.mx";
    s << "\t" << "LABX.colabx";
    s << "\t" << "LABX.susabx";
    s << "\t" << "LABX.susever";
    if (nstates == 3)
    {
        s << "\t" << "LABX.pro";
        s << "\t" << "LABX.proAbx";
        s << "\t" << "LABX.proEver";
    }
    s << "\t" << "LABX.clr";
    s << "\t" << "LABX.clrAbx";
    s << "\t" << "LABX.clrEver";
    return s.str();
}
std::vector<std::string> LinearAbxICP::paramNames() const
{
    std::vector<std::string> res;

    res.push_back("LABX.base");
    res.push_back("LABX.time");
    res.push_back("LABX.mass.mx");
    res.push_back("LABX.freq.mx");
    res.push_back("LABX.colabx");
    res.push_back("LABX.susabx");
    res.push_back("LABX.susever");
    if (nstates == 3)
    {
        res.push_back("LABX.pro");
        res.push_back("LABX.proAbx");
        res.push_back("LABX.proEver");
    }
    res.push_back("LABX.clr");
    res.push_back("LABX.clrAbx");
    res.push_back("LABX.clrEver");

    return res;
}

double LinearAbxICP::getRate(int i, int risk, int ever, int cur) const
{
    return epar[i][0] * ( risk-ever + epar[i][2] * (ever-cur + epar[i][1] * cur));
}

double LinearAbxICP::logProgressionRate(double time, PatientState *p, LocationState *s) const
{
    int everabx =((AbxLocationState *)s)->everAbx((Patient *)p->getOwner());
    int onabx = ((AbxLocationState *)s)->onAbx((Patient *)p->getOwner());
    return log(getRate(1,1,everabx,onabx));
}

double LinearAbxICP::logProgressionGap(double t0, double t1, LocationState *s) const
{
    AbxLocationState *as = (AbxLocationState *) s;
    return -(t1-t0) * getRate(1,as->getLatent(),as->getEverAbxLatent(),as->getAbxLatent());
}

double LinearAbxICP::logClearanceRate(double time, PatientState *p, LocationState *s) const
{
    int everabx =((AbxLocationState *)s)->everAbx((Patient *)p->getOwner());
    int onabx = ((AbxLocationState *)s)->onAbx((Patient *)p->getOwner());
    return log(getRate(2,1,everabx,onabx));
}

double LinearAbxICP::logClearanceGap(double t0, double t1, LocationState *s) const
{
    AbxLocationState *as = (AbxLocationState *) s;
    return -(t1-t0) * getRate(2,as->getColonized(),as->getEverAbxColonized(),as->getAbxColonized());
}

double LinearAbxICP::acqRate(int nsus, int onabx, int everabx, int ncolabx, int ncol, int tot, double time) const
{
    double x = (abs(par[0][1]) < timepartol ? 1 : exp(par[0][1] * (time-tOrigin)));

    double y = (tot > 0 ? epar[0][3]/tot : 0);
    y += 1-epar[0][3];
    y *= epar[0][2]*( (ncol-ncolabx) + epar[0][4]*ncolabx);
    y += 1 - epar[0][2];
    y *= epar[0][0];

    double z = (nsus-everabx) + epar[0][6] * ( (everabx-onabx) + onabx * epar[0][5]);

    return x * y * z;
}

double LinearAbxICP::logAcquisitionRate(double time, PatientState *p, LocationState *ls) const
{
    AbxLocationState *as = (AbxLocationState *) ls;
    int everabx = as->everAbx((Patient *)p->getOwner());
    int onabx = as->onAbx((Patient *)p->getOwner());
    return log(acqRate(1,onabx,everabx,as->getAbxColonized(),as->getColonized(),as->getTotal(),time));
}

double LinearAbxICP::logAcquisitionGap(double u, double v, LocationState *ls) const
{
    AbxLocationState *as = (AbxLocationState *) ls;
    int nsus = as->getSusceptible();
    int neve = as->getEverAbxSusceptible();
    int ncur = as->getAbxSusceptible();
    int ncol = as->getColonized();
    int ntot = as->getTotal();
    int ncax = as->getAbxColonized();

    if (abs(par[0][1]) < timepartol)
    {
        return -(v-u) * acqRate(nsus,ncur,neve,ncax,ncol,ntot,tOrigin);
    }
    else
    {
        return - (acqRate(nsus,ncur,neve,ncax,ncol,ntot,v)-acqRate(nsus,ncur,neve,ncax,ncol,ntot,u)) / par[0][1];
    }
}

double* LinearAbxICP::acquisitionRates(double time, PatientState *p, LocationState *ls) const
{
    AbxLocationState *as = (AbxLocationState *) ls;
    int onabx = as->onAbx((Patient *)p->getOwner());
    int everabx = as->everAbx((Patient *)p->getOwner());

    double *P = new double[nstates];

    if (nstates == 2)
    {
        P[0] = acqRate(1,onabx,everabx,as->getAbxColonized(),as->getColonized(),as->getTotal(),time);
        P[1] = acqRate(1,onabx,everabx,as->getAbxColonized(),1+as->getColonized(),as->getTotal(),time);
    }

    if (nstates == 3)
    {
        P[0] = acqRate(1,onabx,everabx,as->getAbxColonized(),as->getColonized(),as->getTotal(),time);
        P[1] = P[0];
        P[2] = acqRate(1,onabx,everabx,as->getAbxColonized(),1+as->getColonized(),as->getTotal(),time);
    }

    return P;
}

double LinearAbxICP::unTransform(int i, int j)
{
    if (i == 0 && (j == 2 || j == 3))
        return logistic(par[i][j]);
    return exp(par[i][j]);
}

void LinearAbxICP::set(int i, int j, double value, int update, double prival, double priorn)
{
    cout << "LinearAbxICP::set(" << i << "," << j << "," << value << "," << update << "," << prival << "," << priorn << ")\n";
    if (i == 0)
    {
        switch(j)
        {
        case 1:
            setWithLogTransform(i,j,value,update,prival,priorn,0.001);
            break;
        case 2:
        case 3:
            setWithLogitTransform(i,j,value,update,prival,priorn);
            break;
        default:
            setWithLogTransform(i,j,value,update,prival,priorn);
        }

    }
    else
    {
        setWithLogTransform(i,j,value,update,prival,priorn);
    }
}

} // namespace lognormal
