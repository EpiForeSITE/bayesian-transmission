
class MixedICP: public LogNormalICP
{
public:
	MixedICP(int nst, int nmet) : LogNormalICP(nst,8,3,3,nmet) 
	{
	}

	virtual string header()
	{
		stringstream s;
		s << "MICP.base";
		s << "\t" << "MICP.mix";
		s << "\t" << "MICP.ncol";
		s << "\t" << "MICP.ntot";
		s << "\t" << "MICP.time";
		s << "\t" << "MICP.colabx";
		s << "\t" << "MICP.susabx";
		s << "\t" << "MICP.susever";
                if (nstates == 3)
                {
                        s << "\t" << "MICP.pro";
                        s << "\t" << "MICP.proAbx";
                        s << "\t" << "MICP.proEver";
                }
                s << "\t" << "MICP.clr";
                s << "\t" << "MICP.clrAbx";
                s << "\t" << "MICP.clrEver";
		return s.str();
	}

	virtual double getRate(int i, int never, int prev, int cur)
	{
		return epar[i][0] * ( never + epar[i][2] * (prev + epar[i][1] * cur));
	}

	virtual double logProgressionRate(double time, PatientState *p, LocationState *s)
	{
		int everabx =((AbxLocationState *)s)->everAbx((Patient *)p->getOwner());
		int onabx = ((AbxLocationState *)s)->onAbx((Patient *)p->getOwner());
		return log(getRate(1,1-everabx,everabx-onabx,onabx));
	}

	virtual double logProgressionGap(double t0, double t1, LocationState *s)
	{
		AbxLocationState *as = (AbxLocationState *) s;
		int col = as->getColonized();
		int everabx = as->getEverAbxColonized();
		int onabx = as->getAbxColonized();
		return -(t1-t0) * getRate(1,col-everabx,everabx-onabx,onabx);
	}

	virtual double logClearanceRate(double time, PatientState *p, LocationState *s)
	{
		int everabx =((AbxLocationState *)s)->everAbx((Patient *)p->getOwner());
		int onabx = ((AbxLocationState *)s)->onAbx((Patient *)p->getOwner());
		return log(getRate(2,1-everabx,everabx-onabx,onabx));
	}

	virtual double logClearanceGap(double t0, double t1, LocationState *s)
	{
		AbxLocationState *as = (AbxLocationState *) s;
		int col = as->getColonized();
		int everabx = as->getEverAbxColonized();
		int onabx = as->getAbxColonized();
		return -(t1-t0) * getRate(2,col-everabx,everabx-onabx,onabx);
	}


	virtual double acqRate(int nsus, int onabx, int everabx, int ncolabx, int ncol, int tot, double time)
	{
                double x = par[0][0] + par[0][4] * (time -tOrigin);
		x = exp(x);

                double y = ( ncol == 0 ? 0  :  exp( par[0][2] * logint(ncol) + par[0][3] * logint(tot)) );
                y = (1-epar[0][1]) + epar[0][1] * y;
                y = y * exp( par[0][5] * ncolabx);

		double z = (nsus-everabx) + epar[0][7] * ( (everabx-onabx) + onabx * epar[0][6]);

                return y*x*z;
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
		double t = (abs(par[0][4]) < 0.0000001 ? (v-u) : (exp(par[0][4]*(v-tOrigin)) - exp(par[0][4]*(u-tOrigin))) / par[0][4] );

		AbxLocationState *as = (AbxLocationState *) ls;
		int nsus = as->getSusceptible();
		int neve = as->getEverAbxSusceptible();
		int ncur = as->getAbxSusceptible();
                double x = acqRate(nsus,ncur,neve,as->getAbxColonized(),as->getColonized(),as->getTotal(),tOrigin);

		return -t*x;
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
		if (i == 0 && j == 1)
			return logistic(par[i][j]);
		return exp(par[i][j]);
	}
	
	virtual void set(int i, int j, double value, int update, double prival, double priorn)
	{
		if (i == 0)
		{
			switch(j)
			{
			case 1: setWithLogitTransform(i,j,value,update,prival,priorn);
				break;
			case 4: setWithLogTransform(i,j,value,update,prival,priorn,0.001);
				break;
			default:	
				setWithLogTransform(i,j,value,update,prival,priorn);
				break;
			}
		}
		else
		{
			setWithLogTransform(i,j,value,update,prival,priorn);
		}
	}
};
