#include "wrap.h"
#include <Rcpp.h>

using namespace Rcpp;

SEXP params2R(const Parameters * P){
    std::vector<double> values = P->getValues();
    Rcpp::NumericVector rtn(std::begin(values), std::end(values));
    rtn.attr("names") = P->paramNames();
    return rtn;
}

template <> SEXP Rcpp::wrap(const Parameters& P)
{
    return params2R(&P);
}


SEXP model2R(const lognormal::LogNormalModel * model){
    return Rcpp::List::create(
        // _[""] = params2R(model->getParams()),
        _["Insitu"] = params2R(model->getInsituParams()),
        _["SurveilenceTest"] = params2R(model->getSurveilenceTestParams()),
        _["ClinicalTest"] = params2R(model->getClinicalTestParams()),
        _["OutCol"] = params2R(model->getOutColParams()),
        _["InCol"] = params2R(model->getInColParams()),
        _["Abx"] = params2R(model->getAbxParams())
    );
}

template <> SEXP Rcpp::wrap(const lognormal::LogNormalModel& model){
    return model2R(&model);
}
