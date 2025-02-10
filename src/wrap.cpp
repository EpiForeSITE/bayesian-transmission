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


SEXP model2R(const lognormal::LogNormalModel * model)
{

    Rcout << std::endl << "model2R:";
    Rcout << " Insitu";
    auto insitu = params2R(model->getInsituParams());
    Rcout << ", SurveilenceTest";
    auto surveilenceTest = params2R(model->getSurveilenceTestParams());
    Rcout << ", ClinicalTest";
    auto clinicalTest = params2R(model->getClinicalTestParams());
    Rcout << ", OutCol";
    auto outCol = params2R(model->getOutColParams());
    Rcout << ", InCol";
    auto inCol = params2R(model->getInColParams());
    Rcout << ", Abx";
    auto abx = params2R(model->getAbxParams());
    Rcout << std::endl;

    return Rcpp::List::create(
        // _[""] = params2R(model->getParams()),
        _["Insitu"] = insitu, //params2R(model->getInsituParams()),
        _["SurveilenceTest"] = surveilenceTest, //params2R(model->getSurveilenceTestParams()),
        _["ClinicalTest"] = clinicalTest, //params2R(model->getClinicalTestParams()),
        _["OutCol"] = outCol, //params2R(model->getOutColParams()),
        _["InCol"] = inCol, //params2R(model->getInColParams()),
        _["Abx"] = abx //params2R(model->getAbxParams())
    );
}

template <> SEXP Rcpp::wrap(const lognormal::LogNormalModel& model)
{
    return model2R(&model);
}

SEXP Event2R(const infect::Event * e)
{
    return Rcpp::List::create(
        _["time"] = e->getTime(),
        _["type"] = infect::EventCoding::eventString(e->getType()),
        _["patient"] = e->getPatient()->getId(),
        _["unit"] = e->getUnit()->getId(),
        _["facility"] = e->getFacility()->getId()
    );

}
// SEXP HistoryLink2R(const infect::HistoryLink * hl)
// {
//     return Rcpp::List::create(
//         // _["time "]
//     );
//
// }
