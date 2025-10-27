class LogNormalAbxModel : public LogNormalModel
{
public:

	LogNormalAbxModel(int nst, int nmetro, int fw, int ch) : LogNormalModel(nst,nmetro,fw,ch)
	{
		InColParams *icp = getInColParams();
		delete icp;
		icp = new LogNormalAbxICP(nst,0,nmetro);
		setInColParams(icp);
	}
};
