class MultiUnitLinearAbxModel : public LogNormalModel
{
public:

	MultiUnitLinearAbxModel(int nst, List *l, int nmetro, int fw, int ch) : LogNormalModel(nst,nmetro,fw,ch)
	{
		InColParams *icp = getInColParams();
		delete icp;
		icp = new MultiUnitLinearAbxICP(l,nst,nmetro);
		setInColParams(icp);
	}

	virtual void readInColParams(LogNormalICP *icp, istream &is)
	{
		LogNormalModel::readInColParams(icp,is);
		((MultiUnitLinearAbxICP *)icp)->switchOnMultiUnit();
	}
};
