
class LogNormalAbxICP: public LogNormalICP
{

        // Parameters are log rates.
        // Progression parameter is par[1][0].
	// Latent patient on Abx effect on progression is par[1][1].
	// Latent patient ever on Abx effect on progression is par[1][2].
        // Clearance parameter is par[2][0].
	// Colonized patient on Abx effect on clearance is par[2][1].
	// Colonized patient ever on Abx effect on clearance is par[2][2].

	// Acquisition model
	// Time parameter is par[0][0].
	// Constant parameter is par[0][1]
	// log total in-patients parameter is par[0][2]
	// log number colonized parameter is par[0][3]
	// Number colonized parameter is par[0][4]
	// Number abx colonized parameter is par[0][5]
	// Susceptible patient on Abx effect on colonizeation is par[0][6].
	// Susceptible patient ever on Abx effect on colonizeation is par[0][7].

public:
	virtual string header()
	{
		stringstream s;

		s << "LNAX.time";
		s << "\t" "LNAX.base";
		s << "\t" << "LNAX.ltot";
		s << "\t" << "LNAX.lcol";
		s << "\t" << "LNAX.col";
		s << "\t" << "LNAX.colabx";
		s << "\t" << "LNAX.susabx";
		s << "\t" << "LNAX.susever";

		if (nstates == 3)
		{
			s << "\t" << "LNAX.pro";
			s << "\t" << "LNAX.proAbx";
			s << "\t" << "LNAX.proEver";
		}

		s << "\t" << "LNAX.clr";
		s << "\t" << "LNAX.clrAbx";
		s << "\t" << "LNAX.clrEver";

		return s.str();
	}
	
private: 

	virtual double logAcqRate(int onabx, int everabx, int ncolabx, int ncol, int tot, double time)
	{
		double x = 0;

		if (ncol > 0)
			x = par[0][3] * logint(ncol) + par[0][2] * logint(tot);
		else
			x = logint(0);

		x += par[0][0] * (time - tOrigin) + par[0][1] + par[0][4] * ncol + par[0][5] * ncolabx + par[0][6] * onabx + par[0][7] * everabx;
		return x;
	}

	virtual double acqRate(int onabx, int everabx, int ncolabx, int ncol, int tot, double time)
	{
		return exp(logAcqRate(onabx,everabx,ncolabx,ncol,tot,time));
	}

	virtual double logProgRate(int onabx, int ever)
	{
		double x = par[1][0];
		if (onabx)
			x += par[1][1];
		if (ever)
			x += par[1][2];
		return x;
	}

	virtual double logClearRate(int onabx, int ever)
	{
		double x = par[2][0];
		if (onabx)
			x += par[2][1];
		if (ever)
			x += par[2][2];
		return x;
	}

public:

	LogNormalAbxICP(int nst, int isDensity, int nmet) : LogNormalICP(nst,8,3,3,nmet) 
	{
		// Density model

                setNormal(0,0,0,0,0,1,0.001);
                set(0,1,0.001,1,0.001,1);

		switch(isDensity)
		{
		case 0: // Frequency dependent.
			setNormal(0,2,-1,0,0,1);
			setNormal(0,3,1,0,0,1);
			break;

		case 1: // Density dependent.
			setNormal(0,2,0,0,0,1);
			setNormal(0,3,1,0,0,1);
			break;
		
		case 2: // Constant.
			setNormal(0,2,0,0,0,1);
			setNormal(0,3,0,0,0,1);
			break;
		}
	
		setNormal(0,4,0,0,0,1);
		setNormal(0,5,0,0,0,1);
		setNormal(0,6,0,0,0,1);
		setNormal(0,7,0,0,0,1);

	        setNormal(1,0,log(0.01),1,log(0.01),1);
	        setNormal(1,1,0,0,0,1);
	        setNormal(1,2,0,0,0,1);

	        setNormal(2,0,log(0.05),1,log(0.05),1);
	        setNormal(2,1,0,0,0,1);
	        setNormal(2,2,0,0,0,1);
	}


	virtual void set(int i, int j, double v, int u, double pv, double pn)
	{
		if (i == 0 && j == 1)
			setWithLogTransform(i,j,v,u,pv,pn,0.001);
		else
			setWithLogTransform(i,j,v,u,pv,pn);
	}

// Implement LogNormalICP.

	virtual double logProgressionRate(double time, PatientState *p, LocationState *s)
	{
		int onabx = ((AbxLocationState *)s)->onAbx((Patient *)p->getOwner());
		int everabx = ((AbxLocationState *)s)->everAbx((Patient *)p->getOwner());
		return logProgRate(onabx,everabx);
	}

	virtual double logProgressionGap(double t0, double t1, LocationState *s)
	{
		AbxLocationState *as = (AbxLocationState *) s;
                return -(t1-t0) * epar[1][0] *
                        ( as->getNeverAbxLatent() + epar[1][2] * ( (as->getEverAbxLatent() - as->getAbxLatent()) + epar[1][1] * as->getAbxLatent()));
	}

	virtual double logClearanceRate(double time, PatientState *p, LocationState *s)
	{
		int onabx = ((AbxLocationState *)s)->onAbx((Patient *)p->getOwner());
		int everabx = ((AbxLocationState *)s)->everAbx((Patient *)p->getOwner());
		return logClearRate(onabx,everabx);
	}

	virtual double logClearanceGap(double t0, double t1, LocationState *s)
	{
		AbxLocationState *as = (AbxLocationState *) s;
		return -(t1-t0) * epar[2][0] *
                        ( as->getNeverAbxColonized() + epar[2][2] * ( (as->getEverAbxColonized() - as->getAbxColonized()) + epar[2][1] * as->getAbxColonized()));

	}

	virtual inline double logAcquisitionRate(double time, PatientState *p, LocationState *ls)
	{
		AbxLocationState *as = (AbxLocationState *) ls;
		int onabx = as->onAbx((Patient *)p->getOwner());
		int everabx = as->everAbx((Patient *)p->getOwner());
        	return logAcqRate(onabx,everabx,as->getAbxColonized(),as->getColonized(),as->getTotal(),time);
	}

	virtual inline double logAcquisitionGap(double u, double v, LocationState *ls)
	{
		double t = (abs(par[0][0]) < 0.0000001 ? (v-u) : (exp(par[0][0]*(v-tOrigin)) - exp(par[0][0]*(u-tOrigin))) / par[0][0] ) ;
		double x = 0;

		AbxLocationState *as = (AbxLocationState *) ls;

                int inx = as->getNeverAbxSusceptible();
                int ipx = as->getEverAbxSusceptible() - as->getAbxSusceptible();
                int icx = as->getAbxSusceptible();
                x = acqRate(0,0,as->getAbxColonized(),as->getColonized(),as->getTotal(),tOrigin);
                x *= ( inx  + epar[0][7] * ( ipx + icx * epar[0][6])) ;
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
			P[0] = acqRate(onabx,everabx,as->getAbxColonized(),as->getColonized(),as->getTotal(),time);
			P[1] = acqRate(onabx,everabx,as->getAbxColonized(),1+as->getColonized(),as->getTotal(),time);
                }

                if (nstates == 3)
                {
			P[0] = acqRate(onabx,everabx,as->getAbxColonized(),as->getColonized(),as->getTotal(),time);
			P[1] = P[0];
			P[2] = acqRate(onabx,everabx,as->getAbxColonized(),1+as->getColonized(),as->getTotal(),time);
                }

                return P;
	}
};
