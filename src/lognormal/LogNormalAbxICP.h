#ifndef ALUN_LOGNORMAL_LOGNORMALABXICP_H
#define ALUN_LOGNORMAL_LOGNORMALABXICP_H

#include  "LogNormalICP.h"

class LogNormalAbxICP: public LogNormalICP
{

    // Parameters are log rates.
    // Progression parameter is par[1][0].
	// Latent patient on Abx effect on progression is par[1][1].
	// Latent patient ever on Abx effect on progression is par[1][2].
        // Clearance parameter is par[2][0].
	// Colonized patient on Abx effect on clearance is par[2][1].
	// Colonized patient ever on Abx effect on clearance is par[2][2].

	// Acquisition model
	// Time parameter is par[0][0].
	// Constant parameter is par[0][1]
	// log total in-patients parameter is par[0][2]
	// log number colonized parameter is par[0][3]
	// Number colonized parameter is par[0][4]
	// Number abx colonized parameter is par[0][5]
	// Susceptible patient on Abx effect on colonizeation is par[0][6].
	// Susceptible patient ever on Abx effect on colonizeation is par[0][7].
private:
	void setParameterNames();

    inline const double * const logbeta_acq() const{return par[0];}
    inline double logbeta_acq_time() const{return par[0][0];}
    inline double logbeta_acq_constant() const{return par[0][0];}
    inline double logbeta_acq_tot_inpat() const{return par[0][2];}
    inline double logbeta_acq_log_col() const{return par[0][3];}
    inline double logbeta_acq_col() const{return par[0][4];}
    inline double logbeta_acq_abx_col() const{return par[0][5];}
    inline double logbeta_acq_onabx() const{return par[0][6];}
    inline double logbeta_acq_everabx() const{return par[0][7];}

public:
	virtual string header() const override;

protected:

	virtual double timePar() const;

    /// Log Acquisition Rate
    /// \param time Time of acquisition.
    /// \param onabx If patient is on antibiotics at the time.
    /// \param everabx If patient has ever been on antibiotics.
    /// \param ncolabx Number of colonized patients on antibiotics.
    /// \param ncol Number of colonized patients.
    /// \param tot Total number of patients.
	virtual double logAcqRate(int onabx, int everabx, int ncolabx, int ncol, int tot, double time) const;
	double acqRate(double time, int onabx, int everabx, double ncolabx, double ncol, double tot) const;
	virtual double progRate(int onabx, int ever) const;
	virtual double logProgRate(int onabx, int ever) const;
	virtual double clearRate(int onabx, int ever) const;
	virtual double logClearRate(int onabx, int ever) const;

public:

	LogNormalAbxICP(int nst, int isDensity, int nmet, int cap=8);


// Implement LogNormalICP.

	virtual double logProgressionRate(double time, PatientState *p, LocationState *s) const override;
	virtual double logProgressionGap(double t0, double t1, LocationState *s) const override;
	virtual double logClearanceRate(double time, PatientState *p, LocationState *s) const override;
	virtual double logClearanceGap(double t0, double t1, LocationState *s) const override;
	virtual double logAcquisitionRate(double time, PatientState *p, LocationState *ls) const override;
	virtual double logAcquisitionGap(double u, double v, LocationState *ls) const override;
	virtual double *acquisitionRates(double time, PatientState *p, LocationState *ls) const override;
    virtual std::vector<std::string> paramNames() const override;

};
#endif // ALUN_LOGNORMAL_LOGNORMALABXICP_H
