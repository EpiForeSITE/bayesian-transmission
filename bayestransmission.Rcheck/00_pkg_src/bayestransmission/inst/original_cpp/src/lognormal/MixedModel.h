class MixedModel : public LogNormalModel
{
public:

	MixedModel(int nst, int nmetro, int fw, int ch) : LogNormalModel(nst,nmetro,fw,ch)
	{
		InColParams *icp = getInColParams();
		delete icp;
		icp = new MixedICP(nst,nmetro);
		setInColParams(icp);
	}
};
