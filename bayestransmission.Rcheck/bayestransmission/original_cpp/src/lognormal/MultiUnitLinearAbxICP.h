
class MultiUnitLinearAbxICP: public LinearAbxICP
{
protected:

	Map *units;

	int index (Object *u)
	{
		return ((Integer *)units->get(u))->intValue();
	}

public:
	MultiUnitLinearAbxICP(List *l, int nst, int nmet) : LinearAbxICP(nst,nmet,7+l->size()) 
	{
		units = new Map();
		l->init();
		for (int i = 7; i<n[0]; i++)
			units->put(l->next(),new Integer(i));
		n[0] = 7;
	}

	virtual void switchOnMultiUnit()
	{
		n[0] += units->size();

		for (int j=7; j<n[0]; j++)
		{
			par[0][j] = par[0][0];
			epar[0][j] = epar[0][0];
			doit[0][j] = doit[0][0];
			primean[0][j] = primean[0][0];
			pristdev[0][j] = pristdev[0][0];
			sigmaprop[0][j] = sigmaprop[0][0];
		}

		par[0][0] = 0;
		epar[0][0] = 1;
		doit[0][0] = 0;
	}

	virtual string header()
	{
		stringstream s;
		s << "MUABX.base";
		s << "\t" << "MUABX.time";
		s << "\t" << "MUABX.mass.mx";
		s << "\t" << "MUABX.freq.mx";
		s << "\t" << "MUABX.colabx";
		s << "\t" << "MUABX.susabx";
		s << "\t" << "MUABX.susever";

		units->init();
		for (int i=7; i<n[0]; i++)
			s << "\t" << "MUABX." << units->next();

                if (nstates == 3)
                {
                        s << "\t" << "MUABX.pro";
                        s << "\t" << "MUABX.proAbx";
                        s << "\t" << "MUABX.proEver";
                }
                s << "\t" << "MUABX.clr";
                s << "\t" << "MUABX.clrAbx";
                s << "\t" << "MUABX.clrEver";

		return s.str();
	}

	virtual double acqRate(int unit, int nsus, int onabx, int everabx, int ncolabx, int ncol, int tot, double time)
	{
		double x = epar[0][unit] * (abs(par[0][1]) < timepartol ? 1 : exp(par[0][1] * (time-tOrigin)));

		double y = (tot > 0 ? epar[0][3]/tot : 0);
		y += 1-epar[0][3]; 
		y *= epar[0][2]*( (ncol-ncolabx) + epar[0][4]*ncolabx);
		y += 1 - epar[0][2];

		double z = (nsus-everabx) + epar[0][6] * ( (everabx-onabx) + onabx * epar[0][5]);

                return x * y * z;
	}

	virtual inline double logAcquisitionRate(double time, PatientState *p, LocationState *ls)
	{
		AbxLocationState *as = (AbxLocationState *) ls;
		int everabx = as->everAbx((Patient *)p->getOwner());
		int onabx = as->onAbx((Patient *)p->getOwner());
		int unit = index(ls->getOwner());
        	return log(acqRate(unit,1,onabx,everabx,as->getAbxColonized(),as->getColonized(),as->getTotal(),time));
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

		int unit = index(ls->getOwner());

		if (abs(par[0][1]) < timepartol)
		{
			return -(v-u) * acqRate(unit,nsus,ncur,neve,ncax,ncol,ntot,tOrigin);
		}
		else
		{
			return - (acqRate(unit,nsus,ncur,neve,ncax,ncol,ntot,v)-acqRate(unit,nsus,ncur,neve,ncax,ncol,ntot,u)) / par[0][1];
		}
	}

	virtual double *acquisitionRates(double time, PatientState *p, LocationState *ls)
	{
		AbxLocationState *as = (AbxLocationState *) ls;
		int onabx = as->onAbx((Patient *)p->getOwner());
		int everabx = as->everAbx((Patient *)p->getOwner());
		int unit = index(ls->getOwner());

		double *P = new double[nstates];

                if (nstates == 2)
                {
			P[0] = acqRate(unit,1,onabx,everabx,as->getAbxColonized(),as->getColonized(),as->getTotal(),time);
			P[1] = acqRate(unit,1,onabx,everabx,as->getAbxColonized(),1+as->getColonized(),as->getTotal(),time);
                }

                if (nstates == 3)
                {
			P[0] = acqRate(unit,1,onabx,everabx,as->getAbxColonized(),as->getColonized(),as->getTotal(),time);
			P[1] = P[0];
			P[2] = acqRate(unit,1,onabx,everabx,as->getAbxColonized(),1+as->getColonized(),as->getTotal(),time);
                }

                return P;
	}
};
