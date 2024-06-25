class MixedModel : public LogNormalModel
{
public:

	static MixedModel *readModel(istream &is, int nmetro, int forward, int cheat)
	{
		double timsig = 0.001;

		int maxline = 1000;
		char *c = new char[maxline];
		string sdump;
		double p, q, r;
		int up, uq, ur;
		
		int nstates = 0;
		double abxd = 0.0;
		double abxl = 0.0;
		
		is >> sdump >> nstates;

		switch(nstates)
		{
		case 2:
		case 3:
			break;
		default:
			cerr << "Only 2 and 3 states models are allowed.\n";
			exit(1);
		}

		is.getline(c,maxline);

		is >> sdump >> abxd;
		is.getline(c,maxline);

		is >> sdump >> abxl;
		is.getline(c,maxline);
		is.getline(c,maxline);

		MixedModel *model = new MixedModel(nstates,nmetro,forward,cheat);

		model->setAbxDelay(abxd);
		model->setAbxLife(abxl);

	// In situ parameters. 

		InsituParams *isp = model->getInsituParams();
		
		is >> sdump >> p >> up;
		is.getline(c,maxline);

		if (nstates == 2)
		{
			q = 0;
			uq = 0;
		}
		if (nstates == 3)
		{
			is >> sdump >> q >> uq;
			is.getline(c,maxline);
		}

		is >> sdump >> r >> ur;
		is.getline(c,maxline);
		is.getline(c,maxline);

		p = p+q+r;
		r = r/p;
		q = q/p;
		p = 1-q-r;
		up = ur;
		
		isp->set(p,q,r);
		isp->setPriors(p*up,q*up,r*ur);
		isp->setUpdate(up,uq,ur);

	// Surveilence test parameters.

		TestParams *stsp = model->getSurveilenceTestParams();

		is >> sdump >> p >> up;
		is.getline(c,maxline);

		if (nstates == 2)
		{
			q = 0;
			uq = 0;
		}
		if (nstates == 3)
		{
			is >> sdump >> q >> uq;
			is.getline(c,maxline);
		}

		is >> sdump >> r >> ur;
		is.getline(c,maxline);

		stsp->set(0,p,q,r);
		stsp->setPriors(0,p,up,q,uq,r,ur);
		stsp->setUpdate(0,up,uq,ur);

		is.getline(c,maxline);


	//  Clinical test parameters.

		RandomTestParams *ctsp = (RandomTestParams *) model->getClinicalTestParams();

		if (ctsp != stsp)
		{
			is >> sdump >> p >> up;
			is.getline(c,maxline);

			if (nstates == 2)
			{
				q = 0;
				uq = 0;
			}
			if (nstates == 3)
			{
				is >> sdump >> q >> uq;
				is.getline(c,maxline);
			}

			is >> sdump >> r >> ur;
			is.getline(c,maxline);

			ctsp->set(p,q,r);
			ctsp->setPriors(p,up,q,uq,r,ur);
			ctsp->setUpdate(up,uq,ur);

			is >> sdump >> p >> up;
			is.getline(c,maxline);

			if (nstates == 2)
			{
				q = 1;
				uq = 0;
			}
			if (nstates == 3)
			{
				is >> sdump >> q >> uq;
				is.getline(c,maxline);
			}

			is >> sdump >> r >> ur;
			is.getline(c,maxline);
			is.getline(c,maxline);

			ctsp->setRates(p,q,r);	
			ctsp->setUpdateRates(up,uq,ur);
		}
		
	// Out of unit infection parameters.

		OutColParams *ocp = model->getOutColParams();

		is >> sdump >> p >> up;
		is.getline(c,maxline);

		if (nstates == 2)
		{
			q = 0;
			uq = 0;
		}
		if (nstates == 3)
		{
			is >> sdump >> q >> uq;
			is.getline(c,maxline);
		}

		is >> sdump >> r >> ur;
		is.getline(c,maxline);
		is.getline(c,maxline);

		if (nstates == 2)
		{
			ocp->set(p,r);
			ocp->setPriors(p,up,r,ur);
			ocp->setUpdate(up,ur);
		}
		if (nstates == 3)
		{
			ocp->set(p,q,r);
			ocp->setPriors(p,up,q,uq,r,ur);
			ocp->setUpdate(up,uq,ur);
		}

	// In unit infection parameters.

		MixedICP *icp = (MixedICP *) model->getInColParams();

	// Acquisition: indexed by [0][*].
			// time effect: e(0) = 1 for no effect.
			// Set proposal variance small since scale for time is large.
		is >> sdump >> p >> up;
		is.getline(c,maxline);
		icp->set(0,0,p,up,p,up,timsig);

			// constant term: set at rough colonization rate eg 0.001;
		is >> sdump >> p >> up; 
		is.getline(c,maxline);
		icp->set(0,1,p,up,p,up);

			// log total term. exp(0) = 1 for density dependent. exp(-1) = 0.3678794 for freq dependent.
		is >> sdump >> p >> up;
		is.getline(c,maxline);
		icp->set(0,2,p,up,p,up);

			// log ncol term. exp(1) = 2.718282 for density and frequency dependent.
		is >> sdump >> p >> up;
		is.getline(c,maxline);
		icp->set(0,3,p,up,p,up);

			// n colonized patient linear effect: e(0) = 1 for no effect.
		is >> sdump >> p >> up;
		is.getline(c,maxline);
		icp->set(0,4,p,up,p,up);

			// n abx colonized effect: e(0) = 1 for no effect.
		is >> sdump >> p >> up;
		is.getline(c,maxline);
		icp->set(0,5,p,up,p,up);

			// susseptible patient on Abx effect : e(0) = 1 for no effect.
		is >> sdump >> p >> up;
		is.getline(c,maxline);
		icp->set(0,6,p,up,p,up);

			// susseptible patient Ever on Abx effect : e(0) = 1 for no effect.
		is >> sdump >> p >> up;
		is.getline(c,maxline);
		icp->set(0,7,p,up,p,up);

	// Progression: indexed by [1][*].

		if (nstates == 2)
		{
			icp->set(1,0,0,0,0,0);
			icp->set(1,1,0,0,0,0);
			icp->set(1,2,0,0,0,0);
		}

		if (nstates == 3)
		{
				// base  rate
			is >> sdump >> p >> up;
			is.getline(c,maxline);
			icp->set(1,0,p,up,p,up);
				// abx effect
			is >> sdump >> p >> up;
			is.getline(c,maxline);
			icp->set(1,1,p,up,p,up);
				// ever abx effect
			is >> sdump >> p >> up;
			is.getline(c,maxline);
			icp->set(1,2,p,up,p,up);
		}

	// Clearance: indexed by [2][*].
			// base  rate
		is >> sdump >> p >> up;
		is.getline(c,maxline);
		icp->set(2,0,p,up,p,up);
			// abx effect
		is >> sdump >> p >> up;
		is.getline(c,maxline);
		icp->set(2,1,p,up,p,up);
			// ever abx effect
		is >> sdump >> p >> up;
		is.getline(c,maxline);
		icp->set(2,2,p,up,p,up);

		is.getline(c,maxline);

	// Abx parameters.

		AbxParams *abxp = model->getAbxParams();
	
		is >> sdump >> p >> up;
		is.getline(c,maxline);
		if (nstates == 2)
		{
			q = 0;
			uq = 0;
		}

		if (nstates == 3)
		{
			is >> sdump >> q >> uq;
			is.getline(c,maxline);
		}

		is >> sdump >>r >>ur;
		is.getline(c,maxline);

		abxp->set(p,q,r);
		abxp->setUpdate(up,uq,ur);
		abxp->setPriors(p,up,q,uq,r,ur);

	// Done.
		delete [] c;

		return model;
	}

	MixedModel(int nst, int nmetro, int fw = 0, int ch = 0) : LogNormalModel(nst,0,nmetro,fw,ch)
	{
		delete icp;
		icp = new MixedICP(nst,0,nmetro);
		delete survtsp;
		survtsp = new TestParams(nst);
	}
};
