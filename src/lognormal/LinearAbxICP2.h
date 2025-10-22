#ifndef ALUN_LOGNORMAL_LINEARABXICP2_H
#define ALUN_LOGNORMAL_LINEARABXICP2_H

class LinearAbxICP2: public LogNormalICP
{
protected:

    static constexpr double timepartol = 0.000000001;

public:
    using LogNormalICP::set;

    LinearAbxICP2(int nst, int nmet, int nacqpar = 7);
    virtual string header() const override;
    virtual double getRate(int i, int risk, int ever, int cur) const;
    virtual double acqRate(int nsus, int onabx, int everabx, int ncolabx, int ncol, int tot, double time);

    virtual void set(int i, int j, double value, int update, double prival, double priorn);

// LogNormalICP Implementation
    virtual double logProgressionRate(double time, PatientState *p, LocationState *s) override;
    virtual double logProgressionGap(double t0, double t1, LocationState *s) override;
    virtual double logClearanceRate(double time, PatientState *p, LocationState *s) override;
    virtual double logClearanceGap(double t0, double t1, LocationState *s) override;
    virtual double logAcquisitionRate(double time, PatientState *p, LocationState *ls) override;
    virtual double logAcquisitionGap(double u, double v, LocationState *ls) override;
    virtual double *acquisitionRates(double time, PatientState *p, LocationState *ls) override;

    virtual double unTransform(int i, int j) override;

    void debugPrintParams()
    {
        std::cout << "\n=== LinearAbxICP2 Parameters (R Package) ===\n";
        std::cout << "Acquisition params (epar[0]): ";
        for (int j = 0; j < n[0]; j++) {
            std::cout << epar[0][j];
            if (j < n[0]-1) std::cout << ", ";
        }
        std::cout << "\n";
        std::cout << "Progression params (epar[1]): ";
        for (int j = 0; j < n[1]; j++) {
            std::cout << epar[1][j];
            if (j < n[1]-1) std::cout << ", ";
        }
        std::cout << "\n";
        std::cout << "Clearance params (epar[2]): ";
        for (int j = 0; j < n[2]; j++) {
            std::cout << epar[2][j];
            if (j < n[2]-1) std::cout << ", ";
        }
        std::cout << "\n==========================================\n\n";
    }
};

#endif // ALUN_LOGNORMAL_LINEARABXICP2_H
