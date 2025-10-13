#ifndef ALUN_MODELING_TESTPARAMS_H
#define ALUN_MODELING_TESTPARAMS_H

#include "Parameters.h"
#include <vector>
#include <string>
#include <stdexcept>
#include <armadillo>

namespace models {
using arma::uword;
class TestParams : public Parameters
{
protected:

	int nstates;
	uword n;
	uword m;

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
	virtual string header() const override;
	virtual int getNStates() const override;
	virtual double eventProb(InfectionStatus s, int onabx, EventCode e) const;
	virtual double* resultProbs(int onabx, EventCode e) const;

// Implement Parameters.

	virtual double logProb(infect::HistoryLink *h) const override;
	virtual void initCounts() override;
	virtual void count(infect::HistoryLink *h) override;
	virtual void update(Random *r, bool max) override;

// Personal accessors.

	// Set value, update and Beta prior.
	virtual void set(int i, double value, int update, double prival, double prin);
	virtual std::vector<std::string> paramNames() const override;
    virtual std::vector<double> getValues() const override;
    virtual std::vector<double> getLogValues() const;
	virtual void write (ostream &os) const override;
	std::string className() const override { return "TestParams"; }

	virtual int nParam() const
	{
	    return nstates;
	}

	inline void setCount(unsigned int i, unsigned int j, double value)
	{
		if (i < 0 || i >= n || j < 0 || j >= m)
			throw std::out_of_range("Index out of range in TestParams::setCount");
		counts[i][j] = value;
	}
	inline double getCount(unsigned int i,unsigned int j) const
	{
	    if (i < 0 || i >= n || j < 0 || j >= m)
	        throw std::out_of_range("Index out of range in TestParams::getCount");
	    return counts[i][j];
	}
	std::vector<std::vector<double>> getCounts() const
	{
	    std::vector<std::vector<double>> res(n, std::vector<double>(m, 0.0));
	    for (uword i = 0; i < n; i++)
			for (uword j = 0; j < m; j++)
				res[i][j] = getCount(i, j);
		return res;
	}
	void setCounts(arma::mat val)
	{
		if (val.n_rows != n || val.n_cols != m)
			throw std::invalid_argument("Invalid dimensions for counts in TestParams::setCounts");
		for (uword i = 0; i < n; i++)
			for (uword j = 0; j < m; j++)
				counts[i][j] = val(i, j);
	}
};

} // namespace models

#endif // ALUN_MODELING_TESTPARAMS_H
