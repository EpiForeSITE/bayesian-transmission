#ifndef BAYESIAN_TRANSMISSION_MODELSETUP_H
#define BAYESIAN_TRANSMISSION_MODELSETUP_H


#include <string>
using std::string;

#include "util/util.h"
#include "infect/infect.h"
#include "modeling/modeling.h"
#include "lognormal/lognormal.h"

using namespace util;
using namespace infect;
using namespace models;
using namespace lognormal;

#include "wrap.h"

#include <Rcpp.h>
using namespace Rcpp;

#include "RRandom.h"

// void modelsetup(BasicModel * model, Rcpp::List modelParameters);

template <typename T>
inline void setParam(T* x, int i, Rcpp::List Param){
    x->set(i,
           Rcpp::as<double>(Param["init"]),
           Rcpp::as<bool>(Param["update"]),
           Rcpp::as<double>(Param["prior"]),
           Rcpp::as<double>(Param["weight"])
    );
}
template <typename T, typename itype1, typename itype2>
inline void setParam(T* x, itype1 i, itype2 j, Rcpp::List Param){
    x->set(i, j,
           Rcpp::as<double>(Param["init"]),
           Rcpp::as<bool>(Param["update"]),
           Rcpp::as<double>(Param["prior"]),
           Rcpp::as<double>(Param["weight"])
    );
}
template <typename T, typename itype1, typename itype2>
inline void setParamWSig(T* x, itype1 i, itype2 j, Rcpp::List Param){
    x->set(i, j,
           Rcpp::as<double>(Param["init"]),
           Rcpp::as<bool>(Param["update"]),
           Rcpp::as<double>(Param["prior"]),
           Rcpp::as<double>(Param["weight"]),
           Rcpp::as<double>(Param["sigma"])
    );
}

inline void setupInsituParams(
        InsituParams * isp,
        std::vector<double> Probs,
        std::vector<double> Priors,
        std::vector<bool> Doit){
    isp->set(Probs[0], Probs[1], Probs[2]);
    isp->setPriors(Priors[0], Priors[1], Priors[2]);
    isp->setUpdate(Doit[0], Doit[1], Doit[2]);
}

inline void setupInsituParams(
        InsituParams * isp,
        Rcpp::NumericVector Probs,
        Rcpp::NumericVector Priors,
        Rcpp::LogicalVector Doit){
    setupInsituParams(isp,
                      as< std::vector<double> >(Probs),
                      as< std::vector<double> >(Priors),
                      as< std::vector<bool> >(Doit)
    );
}

inline void setupInsituParams(InsituParams * isp, Rcpp::List insituParameters)
{
    setupInsituParams(isp,
                      as< std::vector<double> >(insituParameters["probs"]),
                      as< std::vector<double> >(insituParameters["priors"]),
                      as< std::vector<bool> >(insituParameters["doit"]));
}

inline void setupSurveilenceTestParams(
        TestParams * stp,
        Rcpp::List stpUncolonizedParam,
        Rcpp::List stpColonizedParam,
        Rcpp::List stpRecoveredParam
){
    setParam(stp, 0, stpUncolonizedParam);
    setParam(stp, 1, stpColonizedParam);
    setParam(stp, 2, stpRecoveredParam);
}

inline void setupSurveilenceTestParams(TestParams * stp, Rcpp::List surveilenceTestParameters)
{
    setupSurveilenceTestParams(stp,
                               Rcpp::as<Rcpp::List>(surveilenceTestParameters["uncolonized"]),
                               Rcpp::as<Rcpp::List>(surveilenceTestParameters["colonized"]),
                               Rcpp::as<Rcpp::List>(surveilenceTestParameters["recovered"]));
}


inline void setParamWRate(RandomTestParams* rtp, int i,
                          Rcpp::List p,
                          Rcpp::List r){
    setParam(rtp, false, i, p);
    setParam(rtp, true, i, r);
}
inline void setParamWRate(RandomTestParams* rtp, int i,
                          Rcpp::List pwr){
    setParamWRate(rtp, i, pwr["param"], pwr["rate"]);
}

inline void setupClinicalTestParams(
        RandomTestParams * ctp,
        Rcpp::List ctpUncolonizedParamWRate,
        Rcpp::List ctpColonizedParamWRate,
        Rcpp::List ctpRecoveredParamWRate
){
    setParamWRate(ctp, 0, ctpUncolonizedParamWRate);
    setParamWRate(ctp, 1, ctpUncolonizedParamWRate);
    setParamWRate(ctp, 2, ctpUncolonizedParamWRate);
}

inline void setupClinicalTestParams(RandomTestParams * ctp, Rcpp::List clinicalTestParameters)
{
    setupClinicalTestParams(ctp,
                            Rcpp::as<Rcpp::List>(clinicalTestParameters["uncolonized"]),
                            Rcpp::as<Rcpp::List>(clinicalTestParameters["colonized"]),
                            Rcpp::as<Rcpp::List>(clinicalTestParameters["recovered"]));
}



inline void setupOutOfUnitParams(OutColParams * ocol, Rcpp::List outColParameters)
{
    setParam(ocol, 0, outColParameters["acquisition"]);
    setParam(ocol, 1, outColParameters["clearance"]);
    setParam(ocol, 2, outColParameters[2]);
}

inline void setupLogNormalICPAcquisition(
        LogNormalICP* icp,
        Rcpp::List AcquisitionParams
)
{
    for(auto i=0; i < AcquisitionParams.size(); i++)
    {
        setParam(icp, 0, i, AcquisitionParams[i]);
    }
}

inline void setupLinearAbxAcquisitionModel(
        LinearAbxICP* icp,
        Rcpp::List AcquisitionParams
)
{

    setParam(icp, 0, 0, Rcpp::as<double>(AcquisitionParams["base"]));
    setParam(icp, 0, 1, Rcpp::as<double>(AcquisitionParams["time"]));
    setParam(icp, 0, 2, log(Rcpp::as<double>(AcquisitionParams["mass"])));
    setParam(icp, 0, 3, log(Rcpp::as<double>(AcquisitionParams["freq"])));
    setParam(icp, 0, 4, log(Rcpp::as<double>(AcquisitionParams["col_abx"])));
    setParam(icp, 0, 5, log(Rcpp::as<double>(AcquisitionParams["suss_abx"])));
    setParam(icp, 0, 6, log(Rcpp::as<double>(AcquisitionParams["suss_ever"])));
}

inline void setupAcquisitionParams(
        LogNormalICP * icp,
        Rcpp::List AcquisitionParams
){
    setupLogNormalICPAcquisition(icp, AcquisitionParams);
}

inline void setupAcquisitionParams(
        LinearAbxICP * icp,
        Rcpp::List AcquisitionParams
){
    setupLinearAbxAcquisitionModel(icp, AcquisitionParams);
}

inline void setupProgressionParams (
        LogNormalICP * icp,
        Rcpp::List ProgressionParams
){
    setParam(icp, 1, 0, ProgressionParams["rate"]);
    setParam(icp, 1, 1, ProgressionParams["abx"]);
    setParam(icp, 1, 2, ProgressionParams["ever_abx"]);
}

inline void setupClearanceParams (
        LogNormalICP * icp,
        Rcpp::List ClearanceParams
){
    setParam(icp, 2, 0, ClearanceParams["rate"]);
    setParam(icp, 2, 1, ClearanceParams["abx"]);
    setParam(icp, 2, 2, ClearanceParams["ever_abx"]);
}

inline void setupAbxRateParams(
        AbxParams * abxp,
        Rcpp::List AbxRateParams
){
    setParam(abxp, 0, AbxRateParams["uncolonized"]);
    setParam(abxp, 1, AbxRateParams["colonized"]);
    setParam(abxp, 2, AbxRateParams["recovered"]);
}

template <typename ModelType>
inline void setupAbxParams(
        ModelType * model,
        Rcpp::List AbxParams
){
    model -> setAbx(AbxParams["onoff"], AbxParams["delay"], AbxParams["life"]);
}

template <typename ICPType>
inline void setupInColParams(ICPType * icp, Rcpp::List inColParameters)
{
    setupAcquisitionParams(icp, inColParameters["acquisition"]);
    setupProgressionParams(icp, inColParameters["progression"]);
    setupClearanceParams(icp, inColParameters["clearance"]);
}

template <typename ModelType>
void modelsetup(ModelType * model, Rcpp::List modelParameters, bool verbose = false)
{

    // Antibiotics
    if(verbose) Rcpp::Rcout << std::endl << "  * Setting up Abx...";
    setupAbxParams(model, modelParameters["Abx"]);

    // In situ
    if(verbose) Rcpp::Rcout << "Done" << std::endl
                            << "  * Setting up Insitu...";
    auto isp = model->getInsituParams();
    setupInsituParams(isp, modelParameters["Insitu"]);

    // Surveilence test parameters.
    if(verbose) Rcpp::Rcout << "Done" << std::endl
                            << "  * Setting up Surveilence Test...";
    auto stp = model->getSurveilenceTestParams();
    setupSurveilenceTestParams(stp, modelParameters["SurveilenceTest"]);

    //  Clinical test parameters.
    if(verbose) Rcpp::Rcout << "Done" << std::endl
                            << "  * Setting up Clinical Test...";
    RandomTestParams * ctp = (RandomTestParams *) model->getClinicalTestParams();
    setupClinicalTestParams(ctp, modelParameters["ClinicalTest"]);

    // Out of unit infection parameters.
    if(verbose) Rcpp::Rcout << "Done" << std::endl
                            << "  * Setting up Out of Unit...";
    auto ocol = model->getOutColParams();
    setupOutOfUnitParams(ocol, modelParameters["OutCol"]);

    // In unit infection parameters.
    if(verbose) Rcpp::Rcout << "Done" << std::endl
                            << "  * Setting up In Unit...";
    auto icp = model->getInColParams();
    setupInColParams(icp, modelParameters["InCol"]);

    // Abx rates
    if(verbose) Rcpp::Rcout << "Done" << std::endl
                            << "  * Setting up Abx Rates...";
    auto abxp = model->getAbxParams();
    if(abxp != 0)
    {
        setupAbxRateParams(abxp, modelParameters["AbxRate"]);
    }
    if(verbose) Rcpp::Rcout << "Done" << std::endl;

}

#endif //BAYESIAN_TRANSMISSION_MODELSETUP_H
