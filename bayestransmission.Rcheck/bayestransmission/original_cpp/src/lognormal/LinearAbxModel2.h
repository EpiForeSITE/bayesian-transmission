class LinearAbxModel2 : public LogNormalModel
{
public:

	LinearAbxModel2(int nst, int nmetro, int fw, int ch) : LogNormalModel(nst,nmetro,fw,ch)
	{
		InColParams *icp = getInColParams();
		delete icp;
		icp = new LinearAbxICP2(nst,nmetro);
		setInColParams(icp);
	}
};
