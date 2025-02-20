#ifndef ALUN_MODELING_INSITUPARAMS_H
#define ALUN_MODELING_INSITUPARAMS_H

#include "Parameters.h"
#include <vector>
#include <string>

class InsituParams : public Parameters
{
private:

	int nstates;
	double *probs;
	double *logprobs;
	double *counts;
	double *priors;
	bool *doit;

	virtual inline double prob(InfectionStatus s) const
	{
		int i = stateIndex(s);
		return ( i >=0 ? probs[i] : 0 );
	}

public:

	InsituParams(int nst)
	{
		nstates = nst;

		probs = new double[3];
		logprobs = new double[3];
		counts = new double[3];
		priors = new double[3];
		doit = new bool[3];

		switch(nstates)
		{
		case 2:
			setUpdate(1,0,1);
			set(0.9,0,0.1);
			break;
		case 3:
			set(0.98,0.01,0.01);
			setUpdate(1,1,1);
			break;
		}

		setPriors(1,1,1);
		initCounts();
	}

	InsituParams():nstates(0)
	{
		probs = 0;
		logprobs = 0;
		counts = 0;
		priors = 0;
		doit = 0;
	}

    InsituParams(std::vector<double> probs, std::vector<double> priors, std::vector<bool> doit)
    {
        nstates = probs.size();

        this->probs = new double[nstates];
        this->logprobs = new double[nstates];
        this->counts = new double[nstates];
        this->priors = new double[nstates];
        this->doit = new bool[3];

        for (int i = 0; i < 3; i++)
            {
                this->probs[i] = probs[i];
                this->logprobs[i] = log(probs[i]);
                this->priors[i] = priors[i];
                this->doit[i] = doit[i];
            }

        initCounts();
    }

	~InsituParams()
	{
		if (probs)
			delete [] probs;
		if (logprobs)
			delete [] logprobs;
		if (counts)
			delete [] counts;
		if (priors)
			delete [] priors;
		if (doit)
			delete [] doit;
	}

	virtual inline int nParam() const
	{
		return nstates;
	}

	virtual inline std::vector<std::string> paramNames() const
	{
		std::vector<std::string> res(nstates);

		if (nstates == 3)
		{
			res[0] = "Insit.P(unc)";
			res[1] = "Insit.P(lat)";
			res[2] = "Insit.P(col)";
		}
		if (nstates == 2)
		{
			res[0] = "Insit.P(unc)";
			res[1] = "Insit.P(col)";
		}

		return res;
	}
    virtual std::vector<double> getValues() const
    {
        std::vector<double> vals;
        vals.push_back(probs[0]);
        if(nstates == 3)
            vals.push_back(probs[1]);
        vals.push_back(probs[2]);
        return vals;
    }

	virtual string header()
	{
		auto names = paramNames();
		stringstream s;
		s << names[0] << "\t";
		if (nstates == 3)
			s << names[1] << "\t";
		s << names[2];

		return s.str();
	}

	virtual double *statusProbs() const
	{
		double *P = new double[nstates];

		if (nstates == 2)
		{
			P[0] = prob(uncolonized);
			P[1] = prob(colonized);
		}

		if (nstates == 3)
		{
			P[0] = prob(uncolonized);
			P[1] = prob(latent);
			P[2] = prob(colonized);
		}

		return P;
	}

// Implement Parameters.

	virtual inline double logProb(infect::HistoryLink *h)
	{
		int i = stateIndex(h->getPState()->infectionStatus());
		return ( i >= 0 ? logprobs[i] : 0);
	}

	virtual inline void initCounts()
	{
		for (int i=0; i<3; i++)
			counts[i] = priors[i];
	}

	virtual inline void count(infect::HistoryLink *h)
	{
		int i = stateIndex(h->getPState()->infectionStatus());
		if (i >= 0)
			counts[i] += 1;
	}

	virtual inline void update(Random *r, bool max)
	{
		double t = 0;
		double *cc = new double[3];

		for (int i=0; i<3; i++)
			cc[i] = probs[i];

		if (max)
		{
			for (int i=0; i<3; i++)
			{
				if (doit[i])
					cc[i] = counts[i] - 1;
				t += cc[i];
			}
		}
		else
		{
			for (int i=0; i<3; i++)
			{
				if (doit[i])
					cc[i] = r->rgamma(counts[i],1);
				t += cc[i];
			}
		}

		set(cc[0]/t,cc[1]/t,cc[2]/t);

		delete [] cc;
	}

// Personal accessors.

	inline int getNStates() const
	{
		return nstates;
	}

	inline void set(double u, double l, double c)
	{
		double tot = (u+l+c);
		probs[0] = u/tot;
		probs[1] = l/tot;
		probs[2] = c/tot;
		for (int i=0; i<3; i++)
			logprobs[i] = log(probs[i]);
	}

    /// Set the prior probabilities for the states.
	inline void setPriors(
	        double pu, /// Prior probability of uncolonized
	        double pl, /// Prior probability of latent
	        double pc) /// Prior probability of colonized
	{
		// Relative values of probs, sum is equivalent number of obs.
		priors[0] = pu;
		priors[1] = pl;
		priors[2] = pc;
	}

	inline void setUpdate(bool u, bool l, bool c)
	{
		doit[0] = u;
		doit[1] = l;
		doit[2] = c;
	}

	virtual void write(ostream &os)
	{
		char *buffer = new char[100];
		sprintf(buffer,"%12.10f\t",probs[0]);
		os << buffer;

		if (nstates == 3)
		{
			sprintf(buffer,"%12.10f\t",probs[1]);
			os << buffer;
		}

		sprintf(buffer,"%12.10f",probs[2]);
		os << buffer;

		delete [] buffer;
	}

    virtual void init(double p, bool up,
                      double q, bool uq,
                      double r, bool ur)
    {
        double tot = p+q+r;
        r = r/tot;
        q = q/tot;
        p = 1-q-r;
        //up = ur;

        set(p,q,r);
        setPriors(up?p:0, uq?q:0, ur?r:0);
        setUpdate((up>0),(uq>0),(ur>0));
    }
    static void skipLine(istream &is)
    {
        char c;
        do
        {
            c = is.get();
        }
        while (c != '\n' && c != EOF);
    }
    virtual inline void read(istream &is)
    {
        string sdump;
        double p,q=0,r;
        double up, uq=0, ur;
        is >> sdump >> p >> up;
        skipLine(is);

        if (nstates == 3)
        {
            is >> sdump >> q >> uq;
            skipLine(is);
        }
        is >> sdump >> r >> ur;
        skipLine(is);

        init(p, up, q, uq, r, ur);
    }

};
#endif // ALUN_MODELING_INSITUPARAMS_H
