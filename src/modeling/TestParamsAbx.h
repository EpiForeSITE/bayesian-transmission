#ifndef ALUN_MODELING_TESTPARAMSABX_H
#define ALUN_MODELING_TESTPARAMSABX_H

#include "TestParams.h"

namespace models {

class TestParamsAbx : public TestParams
{
protected:

	int nstates;
	int l;
	int m;
	int n;

	// l x m x n array with probs[i][j][k] = P(test result == k | colonization status = i, abx status = j)

	double ***probs;
	double ***logprobs;
	double ***counts;
	double ***priors;
	int **doit;

	bool useabx;

	virtual void set(int i, int j, double value)
	{
		probs[i][j][0] = 1-value;
		probs[i][j][1] = value;
		logprobs[i][j][0] = log(probs[i][j][0]);
		logprobs[i][j][1] = log(probs[i][j][1]);
	}

public:

	TestParamsAbx(int nst, bool abx);
	~TestParamsAbx();

	virtual std::vector<std::string> paramNames() const;
	inline void setUseAbx(bool i){useabx = i;}
	inline bool getUseAbx(){return useabx;}
	virtual double eventProb(InfectionStatus s, int onabx, EventCode e) const;

	virtual double *resultProbs(int onabx, EventCode e) const;

// Implement Parameters.

	virtual double logProb(infect::HistoryLink *const h) const;
	virtual void initCounts();
	virtual void count(infect::HistoryLink * const h);
	virtual void update(Random *r, bool max);

// Personal accessors.

	// Set value, update, and Beta priors.
	virtual void set(int i, int j, double value, int update, double prival, double prin);
	virtual int nParam() const;
	virtual void write (ostream &os) const;
    virtual std::vector<double> getValues() const;
};

} // namespace models

#endif //ALUN_MODELING_TESTPARAMSABX_H
