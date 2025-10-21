class LTCFModel : public LogNormalModel
{
public:

	LTCFModel(int nst, int nmetro, int fw=0, int ch=0) : LogNormalModel(nst,fw,ch)
	{
		abxbyonoff = 1;
                isp = new InsituParams(nstates);
                survtsp = new TestParams(nstates);
                clintsp = new RandomTestParams(nstates);
                ocp = new OutColParams(nstates,nmetro);
		icp = new LTCFInColParams(nst,nmetro);
	}

	virtual int needEventType(EventCode e)
	{
		switch(e)
		{
		case insitu:
		case insitu0:
		case insitu1:
		case insitu2:
		case admission:
		case admission0:
		case admission1:
		case admission2:
		case discharge:
		case negsurvtest:
		case possurvtest:
		case negclintest:
		case posclintest:
		case isolon:
		case isoloff:
			return 1;
		default:
			return 0;
		}
	}

	virtual PatientState *makePatientState(Patient *p)
	{
		return p == 0 ? 0 : new LTCFPatientState(p,nstates);
	}

	virtual LocationState *makeUnitState(Unit *u)
	{
		return u == 0 ? 0 : new LTCFLocationState(u,nstates);
	}

	virtual void read(istream &is)
	{
		skipLine(is);
		readInsituParams(getInsituParams(),is);
		skipLine(is);
		readTestParams(getSurveilenceTestParams(),is);
		skipLine(is);
		readRandomTestParams((RandomTestParams *) getClinicalTestParams(),is);
		skipLine(is);
		readOutColParams(getOutColParams(),is);
		skipLine(is);
		readInColParams((LogNormalICP *)getInColParams(),is);
		skipLine(is);
	}
};
