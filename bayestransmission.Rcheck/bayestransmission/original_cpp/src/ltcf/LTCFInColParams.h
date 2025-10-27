
class LTCFInColParams: public LogNormalICP
{
public:
	LTCFInColParams(int nst, int nmet) : LogNormalICP(nst,9,1,1,nmet) 
	{
	}

	virtual string header()
	{
		stringstream s;

		s << "" << "In.Base0";
		s << "\t" << "In.Base1";
		s << "\t" << "In.Trans00";
		s << "\t" << "In.Trans01";
		s << "\t" << "In.Trans10";
		s << "\t" << "In.Trans11";
		s << "\t" << "In.ColIsol";
		s << "\t" << "In.SusIsol";
		s << "\t" << "In.SusIsolEver";

                if (nstates == 3)
                        s << "\t" << "In.Prog";
                s << "\t" << "In.Clear";

		return s.str();
	}

	virtual double acqRate(int s0, int s1, int si0, int si1, int se0, int se1, int c0, int c1, int ci0, int ci1)
	{
		double x0 = (c0 - ci0) + epar[0][6] * ci0;
		double x1 = (c1 - ci1) + epar[0][6] * ci1;

		double y0 = (s0 - se0) + epar[0][8] * ( (se0 - si0) + si0 * epar[0][7] );
		double y1 = (s1 - se1) + epar[0][8] * ( (se1 - si1) + si1 * epar[0][7] );

		//double y0 = (s0 - se0) + si0 * epar[0][7] + epar[0][8] * (se0 - si0);
		//double y1 = (s1 - se1) + si1 * epar[0][7] + epar[0][8] * (se1 - si1);

		return y0*epar[0][0] + y1*epar[0][1] + x0*y0*epar[0][2] + x0*y1*epar[0][3] + x1*y0*epar[0][4] + x1*y1*epar[0][5];
	}

	virtual inline double logAcquisitionRate(double time, PatientState *p, LocationState *ls)
	{
		LTCFLocationState *as = (LTCFLocationState *) ls;
		Patient *pat = (Patient *) p->getOwner();

		int g = pat->getGroup();

		int s0 = (1-g);
		int s1 = g;
		int si0 = (1-g)*as->inIsol(pat);
		int si1 = g*as->inIsol(pat);
		int se0 = (1-g)*as->everIsol(pat);
		int se1 = g*as->everIsol(pat);

		int c0 = as->getColonized(0);
		int c1 = as->getColonized(1);
		int ci0 = as->getIsolColonized(0);
		int ci1 = as->getIsolColonized(1);

        	return log(acqRate(s0,s1,si0,si1,se0,se1,c0,c1,ci0,ci1));
	}

	virtual inline double logAcquisitionGap(double u , double v, LocationState *ls)
	{
		LTCFLocationState *as = (LTCFLocationState *) ls;

		int s0 = as->getSusceptible(0);
		int s1 = as->getSusceptible(1);

		int si0 = as->getIsolSusceptible(0);
		int si1 = as->getIsolSusceptible(1);
		int se0 = as->getEverIsolSusceptible(0);
		int se1 = as->getEverIsolSusceptible(1);

		int c0 = as->getColonized(0);
		int c1 = as->getColonized(1);
		int ci0 = as->getIsolColonized(0);
		int ci1 = as->getIsolColonized(1);

        	return - (v-u) * acqRate(s0,s1,si0,si1,se0,se1,c0,c1,ci0,ci1);
	}

	virtual double *acquisitionRates(double time, PatientState *p, LocationState *ls)
	{
		LTCFLocationState *as = (LTCFLocationState *) ls;
		Patient *pat = (Patient *) p->getOwner();

		int g = pat->getGroup();

		int s0 = (1-g);
		int s1 = g;
		int si0 = (1-g)*as->inIsol(pat);
		int si1 = g*as->inIsol(pat);
		int se0 = (1-g)*as->everIsol(pat);
		int se1 = g*as->everIsol(pat);

		int c0 = as->getColonized(0);
		int c1 = as->getColonized(1);
		int ci0 = as->getIsolColonized(0);
		int ci1 = as->getIsolColonized(1);

		double *P = new double[nstates];

                if (nstates == 2)
                {
			P[0] = acqRate(s0,s1,si0,si1,se0,se1,c0,c1,ci0,ci1);
			P[1] = acqRate(s0,s1,si0,si1,se0,se1,(1-g)+c0,g+c1,ci0,ci1);
                }

                if (nstates == 3)
                {
			P[0] = acqRate(s0,s1,si0,si1,se0,se1,c0,c1,ci0,ci1);
			P[1] = P[0];
			P[2] = acqRate(s0,s1,si0,si1,se0,se1,(1-g)+c0,g+c1,ci0,ci1);
                }

                return P;
	}

	virtual double logProgressionRate(double time, PatientState *p, LocationState *s)
	{
		return log(epar[1][0]);
	}

	virtual double logProgressionGap(double u, double v, LocationState *s)
	{
		return -(v-u) * epar[1][0]  * s->getLatent();
	}

	virtual double logClearanceRate(double time, PatientState *p, LocationState *s)
	{
		return log(epar[2][0]);
	}

	virtual double logClearanceGap(double u, double v, LocationState *s)
	{
		return -(v-u) * epar[2][0]  * s->getColonized();
	}

	virtual double unTransform(int i, int j)
	{
		return exp(par[i][j]);
	}
	
	virtual void set(int i, int j, double value, int update, double prival, double priorn)
	{
		setWithLogTransform(i,j,value,update,prival,priorn);
	}
};
