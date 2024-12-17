#ifndef ALUN_LOGNORMAL_LINEARABXICP_H
#define ALUN_LOGNORMAL_LINEARABXICP_H

#include "LogNormalICP.h"

class LinearAbxICP: public LogNormalICP
{
protected:

    static constexpr double timepartol = 0.000000001;

public:
    LinearAbxICP(int nst, int nmet, int nacqpar = 7) : LogNormalICP(nst,nacqpar,3,3,nmet)
    {
    }

    virtual string header()
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

    virtual double getRate(int i, int risk, int ever, int cur)
    {
        return epar[i][0] * ( risk-ever + epar[i][2] * (ever-cur + epar[i][1] * cur));
    }

    virtual double logProgressionRate(double time, PatientState *p, LocationState *s)
    {
        int everabx =((AbxLocationState *)s)->everAbx((Patient *)p->getOwner());
        int onabx = ((AbxLocationState *)s)->onAbx((Patient *)p->getOwner());
        return log(getRate(1,1,everabx,onabx));
    }

    virtual double logProgressionGap(double t0, double t1, LocationState *s)
    {
        AbxLocationState *as = (AbxLocationState *) s;
        return -(t1-t0) * getRate(1,as->getLatent(),as->getEverAbxLatent(),as->getAbxLatent());
    }

    virtual double logClearanceRate(double time, PatientState *p, LocationState *s)
    {
        int everabx =((AbxLocationState *)s)->everAbx((Patient *)p->getOwner());
        int onabx = ((AbxLocationState *)s)->onAbx((Patient *)p->getOwner());
        return log(getRate(2,1,everabx,onabx));
    }

    virtual double logClearanceGap(double t0, double t1, LocationState *s)
    {
        AbxLocationState *as = (AbxLocationState *) s;
        return -(t1-t0) * getRate(2,as->getColonized(),as->getEverAbxColonized(),as->getAbxColonized());
    }

    virtual double acqRate(int nsus, int onabx, int everabx, int ncolabx, int ncol, int tot, double time)
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

    virtual inline double logAcquisitionRate(double time, PatientState *p, LocationState *ls)
    {
        AbxLocationState *as = (AbxLocationState *) ls;
        int everabx = as->everAbx((Patient *)p->getOwner());
        int onabx = as->onAbx((Patient *)p->getOwner());
        return log(acqRate(1,onabx,everabx,as->getAbxColonized(),as->getColonized(),as->getTotal(),time));
    }

    virtual inline double logAcquisitionGap(double u, double v, LocationState *ls)
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

    virtual double *acquisitionRates(double time, PatientState *p, LocationState *ls)
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

    virtual double unTransform(int i, int j)
    {
        if (i == 0 && (j == 2 || j == 3))
            return logistic(par[i][j]);
        return exp(par[i][j]);
    }

    virtual void set(int i, int j, double value, int update, double prival, double priorn)
    {
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
};


#endif // ALUN_LOGNORMAL_LINEARABXICP_H
