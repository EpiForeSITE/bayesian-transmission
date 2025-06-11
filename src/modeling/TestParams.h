#ifndef ALUN_MODELING_TESTPARAMS_H
#define ALUN_MODELING_TESTPARAMS_H

#include "Parameters.h"
#include <vector>
#include <string>
#include <stdexcept>
#include <armadillo>

namespace models {

class TestParams : public Parameters
{
protected:

	int nstates;
	int n;
	int m;

	// nxm matrix with probs[i][j] = P(test result == j | colonization status = i)
	double** probs;
	double** logprobs;
	double** counts;
	double** priors;
	int* doit;

	virtual void set(int i, double value)
	{
		probs[i][0] = 1-value;
		probs[i][1] = value;
		logprobs[i][0] = log(probs[i][0]);
		logprobs[i][1] = log(probs[i][1]);
	}

public:

	TestParams(int nst);
	~TestParams();
	virtual string header() const;
	virtual int getNStates() const;
	virtual double eventProb(InfectionStatus s, int onabx, EventCode e) const;
	virtual double* resultProbs(int onabx, EventCode e) const;

// Implement Parameters.

	virtual double logProb(infect::HistoryLink *h) const;
	virtual void initCounts();
	virtual void count(infect::HistoryLink *h);
	virtual void update(Random *r, bool max);

// Personal accessors.

	// Set value, update and Beta prior.
	virtual void set(int i, double value, int update, double prival, double prin);
	virtual std::vector<std::string> paramNames() const;
    virtual std::vector<double> getValues() const;
	virtual void write (ostream &os) const;

	virtual int nParam() const
	{
	    return nstates;
	}

	void setCount(int i, int j, double value)
	{
		if (i < 0 || i >= n || j < 0 || j >= m)
			throw std::out_of_range("Index out of range in TestParams::setCount");
		counts[i][j] = value;
	}
	arma::mat getCounts() const
	{
	    arma::mat res(n, m);
		for (int i = 0; i < n; i++)
			for (int j = 0; j < m; j++)
				res(i, j) = counts[i][j];
		return res;
	}
	void setCounts(arma::mat val)
	{
		if (val.n_rows != n || val.n_cols != m)
			throw std::invalid_argument("Invalid dimensions for counts in TestParams::setCounts");
		for (int i = 0; i < n; i++)
			for (int j = 0; j < m; j++)
				counts[i][j] = val(i, j);
	}
	double getCount(int i, int j) const
	{
		if (i < 0 || i >= n || j < 0 || j >= m)
			throw std::out_of_range("Index out of range in TestParams::getCount");
		return counts[i][j];
	}
};

} // namespace models

#endif // ALUN_MODELING_TESTPARAMS_H
