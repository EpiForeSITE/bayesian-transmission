
class MetroParameterSampler : public ParameterSampler
{
private:
	double alphasig;
	double betasig;
	double impsig;
	double ratesig;

	Likelihood *l;

public:

	MetroParameterSampler(History *h, Model *mod, Random *rand) : ParameterSampler(h,mod,rand)
	{
		l = new Likelihood(h,mod);
		alphasig = 1;
		betasig = 0.001;
		impsig = 1.0;
		ratesig = 1.0;
	}

	MetroParameterSampler(Model **mods, int nmods, EpisodeEvent **pts, Random *rand) : ParameterSampler(mods,nmods,pts,rand)
	{
		l = new Likelihood(mods,nmods,pts);
		alphasig = 1;
		betasig = 0.01;
		impsig = 1.0;
		ratesig = 1.0;
	}

	Likelihood *getLikelihood()
	{
		return l;
	}

	void sampleImportation(int max, int nmetro, double ia, double ib, double ra, double rb)
	{
		double imp = m[0]->getImportation();
		double rat = m[0]->getOutDecol();

		double f = log(imp*(1-imp)) + log(rat) + l->logLikeAdmit();

		if (!max)
		{
			//if (ia > 0 && ib > 0)
				f += r->logdbeta(imp,ia,ib);
			//if (ra > 0 && rb > 0)
				f += r->logdgamma(rat,ra,rb);
		}

		for (int i=0; i<nmetro; i++)
		{
			double newimp = invlogit(logit(imp) + r->rnorm(0,impsig));
			double newrat = exp(log(rat) + r->rnorm(0,ratesig));

			for (int i=0; i<n; i++)
				m[i]->setImportation(newimp,newrat);

			double newf  = log(newimp*(1-newimp)) + log(newrat) + l->logLikeAdmit(); 

			if (!max)
			{
				if (ia > 0 && ib > 0)
					newf += r->logdbeta(newimp,ia,ib); 
				if (ra > 0 && rb > 0)
					newf += r->logdgamma(newrat,ra,rb);
			}

			if ((max ? 0 : log(r->runif())) < newf - f)
			{
				imp = newimp;
				rat = newrat;
				f = newf;
			}
			else
			{
				for (int i=0; i<n; i++)
					m[i]->setImportation(imp,rat);
			}

		}
	}

	void sampleOutColDecol(int i, int max, int nmetro, double ia, double ib, double ra, double rb)
	{
		double imp = m[i]->getImportation();
		double rat = m[i]->getOutDecol();
		double f = log(imp*(1-imp)*rat) + l->logLikeAdmit(i);

		if (!max)
		{
			if (ia > 0 && ib > 0)
				f += r->logdbeta(imp,ia,ib);
			if (ra > 0 && rb > 0)
				f += r->logdgamma(rat,ra,rb);
		}

		for (int j=0; j<nmetro; j++)
		{
			double newimp = invlogit(logit(imp) + r->rnorm(0,impsig));
			double newrat = exp(log(rat) + r->rnorm(0,ratesig));

			m[i]->setImportation(newimp,newrat);

			double newf = log(newimp*(1-newimp)*newrat) + l->logLikeAdmit(i); 

			if (!max)
			{
				if (ia > 0 && ib > 0)
					newf += r->logdbeta(newimp,ia,ib);
				if (ra > 0 && rb > 0)
					newf += r->logdgamma(newrat,ra,rb);
			}

			if ((max ? 0 : log(r->runif())) < newf - f)
			{
				imp = newimp;
				rat = newrat;
				f = newf;
			}
			else
			{
				m[i]->setImportation(imp,rat);
			}
		}
	}

	void sampleColonization(int max, int nmetro, double imu, double isig, double smu, double ssig)
	{
		double alpha = m[0]->getColIntercept();
		double beta = m[0]->getColSlope();

		double f = max ? 0 : r->logdnorm(alpha,imu,isig) + r->logdnorm(beta,smu,ssig);
		f += l->logLike();

		for (int j=0; j<nmetro; j++)
		{
			double newalpha = alpha + r->rnorm(0,alphasig);
			double newbeta = beta + r->rnorm(0,betasig);
			for (int i=0; i<n; i++)
				m[i]->setColIntSlope(newalpha,newbeta);

			double newf = max ? 0 : r->logdnorm(newalpha,imu,isig) + r->logdnorm(newbeta,smu,ssig);
			newf += l->logLike();

			if ( (max ? 0 : log(r->runif())) < newf - f)
			{
				alpha = newalpha;
				beta = newbeta;
				f = newf;
			}
			else
			{
				for (int i=0; i<n; i++)
					m[i]->setColIntSlope(alpha,beta);
			}
		}
	}

	void sampleColonization(int i, int max, int nmetro, double imu, double isig, double smu, double ssig)
	{
		double alpha = m[i]->getColIntercept();
		double beta = m[i]->getColSlope();

		double f = max ? 0 : r->logdnorm(alpha,imu,isig) + r->logdnorm(beta,smu,ssig);
		f += l->logLikeCol(i);

		for (int j=0; j<nmetro; j++)
		{
			double newalpha = alpha + r->rnorm(0,alphasig);
			double newbeta = beta + r->rnorm(0,betasig);
			m[i]->setColIntSlope(newalpha,newbeta);

			double newf = max ? 0 : r->logdnorm(newalpha,imu,isig) + r->logdnorm(newbeta,smu,ssig);
			newf += l->logLikeCol(i);

			if ( (max ? 0 : log(r->runif())) < newf - f)
			{
				alpha = newalpha;
				beta = newbeta;
				f = newf;
			}
			else
			{
				m[i]->setColIntSlope(alpha,beta);
			}
		}
	}

	void sampleColonization(int max, int nmetro, double imu, double isig)
	{
		double alpha = m[0]->getColIntercept();

		double f = max ? 0 : r->logdnorm(alpha,imu,isig);
		f += l->logLikeCol();

		for (int j=0; j<nmetro; j++)
		{
			double newalpha = alpha + r->rnorm(0,alphasig);
			for (int i=0; i<n; i++)
				m[i]->setColIntSlope(newalpha,0);

			double newf = max ? 0 : r->logdnorm(newalpha,imu,isig);
			newf += l->logLikeCol();

			if ( (max ? 0 : log(r->runif())) < newf - f)
			{
				alpha = newalpha;
				f = newf;
			}
			else
			{
				for (int i=0; i<n; i++)
					m[i]->setColIntSlope(alpha,0);
			}
		}
	}

	void sampleColonization(int i, int max, int nmetro, double imu, double isig)
	{
		double alpha = m[i]->getColIntercept();

		double f = max ? 0 : r->logdnorm(alpha,imu,isig);
		f += l->logLikeCol(i);

		for (int j=0; j<nmetro; j++)
		{
			double newalpha = alpha + r->rnorm(0,alphasig);
			m[i]->setColIntSlope(newalpha,0);

			double newf = max ? 0 : r->logdnorm(newalpha,imu,isig);
			newf += l->logLikeCol(i);

			if ( (max ? 0 : log(r->runif())) < newf - f)
			{
				alpha = newalpha;
				f = newf;
			}
			else
			{
				m[i]->setColIntSlope(alpha,0);
			}
		}
	}

};
