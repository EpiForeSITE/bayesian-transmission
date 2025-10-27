
class LNMassActionModel : public LogNormalModel
{
public:

	LNMassActionModel(int nst, int nmetro, int fw, int ch) : LogNormalModel(nst,nmetro,fw,ch)
	{
		InColParams *icp = getInColParams();
		delete icp;
		icp = new LNMassActionICP(nst,0,nmetro);
		setInColParams(icp);

		delete clintsp;
		clintsp = 0;

		delete abxp;
		abxp = 0;
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
			return 1;

		case abxdose:
			return !abxbyonoff;

		case abxon:
		case abxoff:
			return abxbyonoff;

		case acquisition:
		case progression:
		case clearance:
			return cheating;

		default:
			return 0;
		}
	}
};
