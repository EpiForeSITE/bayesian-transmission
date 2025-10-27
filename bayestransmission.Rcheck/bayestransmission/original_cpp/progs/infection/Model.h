
class Model : public Object
{
friend class Likelihood;

protected:
	double alpha;
	double beta;
	double t0;

	double pinsitu;
	double logoneminpin;
	double logpin;

	double import;
	double outrate;

	double fpos;
	double logfpos;
	double logtneg;

	double fneg;
	double logfneg;
	double logtpos;

	double decol;
	double logdecol;

	string *names;
	int npar;

	void setNames()
	{
		npar = 8;

		names = new string[npar];
		names[0] = "Prob insitu";
		names[1] = "Importation";
		names[2] = "Out Rate";
		names[3] = "Col Intercept";
		names[4] = "Col Slope";
		names[5] = "Decolonization";
		names[6] = "False Pos"; 
		names[7] = "False Neg";
	}

public:
	Model()
	{
		setProbInsitu(0);
		setImportation(0,0);
		setColIntSlope(0,0);
		setDecolonization(0);
		setFalsePos(0);
		setFalseNeg(0);
		setNames();
		t0 = 0;
	}

	Model(double *p)
	{
		setNames();
		setParameters(p);
		t0 = 0;
	}

	Model(double pin, double imp, double orate, double coli, double cols, double decol, double fpos, double fneg)
	{
		setNames();
		setProbInsitu(pin);
		setImportation(imp,orate);
		setColIntSlope(coli,cols);
		setDecolonization(decol);
		setFalsePos(fpos);
		setFalseNeg(fneg);
		t0 = 0;
	}

	double instCol(double t)
	{
		return exp(alpha+beta*(t-t0));
	}

	double cumCol(double t1, double t2)
	{
		if (beta > -0.000000001 && beta < 0.000000001)
		{
			return exp(alpha) * (t2-t1);
		}
		else
		{
			return ( exp(alpha + beta*(t2-t0)) - exp(alpha + beta*(t1-t0)) ) /beta;
		}
	}

	void setInterceptTime(double t)
	{
		t0 = t;
	}

	void setParameters(double *pars)
	{
		setProbInsitu(pars[0]);
		setImportation(pars[1],pars[2]);
		setColIntSlope(pars[3],pars[4]);
		setDecolonization(pars[5]);
		setFalsePos(pars[6]);
		setFalseNeg(pars[7]);
	}

	void getParameters(double *pars)
	{
		pars[0] = getProbInsitu();
		pars[1] = getImportation();
		pars[2] = getOutDecol();
		pars[3] = getColIntercept();
		pars[4] = getColSlope();
		pars[5] = getDecolonization();
		pars[6] = getFalsePos();
		pars[7] = getFalseNeg();
	}

	int nParameters()
	{
		return npar;
	}

	string *getNames()
	{
		return names;
	}

	void setProbInsitu(double p)
	{
		pinsitu = p;
		logpin = log(pinsitu);
		logoneminpin = log(1-pinsitu);
	}

	void setImportation(double t)
	{
		import = t;
	}

	void setImportation(double i, double r)
	{
		import = i;
		outrate = r;
	}

	double getOutDecol()
	{
		return outrate;
	}

	double getImportation()
	{
		return import;
	}

	double getImportation(int s, double t)
	{
		return import + exp(-t*outrate) * ( s == 0 ? -import : (1-import) ) ;
	}

	void setColonization(double t)
	{
		setColIntSlope(log(t),0);
	}

	double getColonization()
	{
		return exp(alpha);
	}

	void setColIntSlope(double a, double b)
	{
		alpha = a;
		beta = b;
	}

	double getColIntercept()
	{
		return alpha;
	}
	
	double getColSlope()
	{
		return beta;
	}

	void setDecolonization(double t)
	{
		decol = t;
		logdecol = log(decol);
	}

	double getDecolonization()
	{
		return decol;
	}

	void setFalsePos(double t)
	{
		fpos = t;
		logfpos = log(fpos);
		logtneg = log(1-fpos);
	}

	void setFalseNeg(double t)
	{
		fneg = t;
		logfneg = log(fneg);
		logtpos = log(1-fneg);
	}

	double getProbInsitu()
	{
		return pinsitu;
	}

	double getFalsePos()
	{
		return fpos;
	}

	double getFalseNeg()
	{
		return fneg;
	}

	string className()
	{
		return "Model";
	}

	void write(ostream &os)
	{
		os << "Model(" << pinsitu << "," << import << "," << outrate << "," << alpha << "," << beta 
			<< "," << decol << "," << fpos << "," << fneg << ")";
	}
};
