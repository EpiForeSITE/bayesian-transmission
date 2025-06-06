// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <RcppEigen.h>
#include <Rcpp.h>

using namespace Rcpp;

#ifdef RCPP_USE_GLOBAL_ROSTREAM
Rcpp::Rostream<true>&  Rcpp::Rcout = Rcpp::Rcpp_cout_get();
Rcpp::Rostream<false>& Rcpp::Rcerr = Rcpp::Rcpp_cerr_get();
#endif

// CodeToEvent
CharacterVector CodeToEvent(IntegerVector x);
RcppExport SEXP _bayestransmission_CodeToEvent(SEXP xSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< IntegerVector >::type x(xSEXP);
    rcpp_result_gen = Rcpp::wrap(CodeToEvent(x));
    return rcpp_result_gen;
END_RCPP
}
// EventToCode
std::vector<int> EventToCode(const std::vector<std::string> x);
RcppExport SEXP _bayestransmission_EventToCode(SEXP xSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const std::vector<std::string> >::type x(xSEXP);
    rcpp_result_gen = Rcpp::wrap(EventToCode(x));
    return rcpp_result_gen;
END_RCPP
}
// runMCMC
SEXP runMCMC(std::string modname, Rcpp::DataFrame data, Rcpp::List MCMCParameters, Rcpp::List modelParameters, int nstates, bool verbose);
RcppExport SEXP _bayestransmission_runMCMC(SEXP modnameSEXP, SEXP dataSEXP, SEXP MCMCParametersSEXP, SEXP modelParametersSEXP, SEXP nstatesSEXP, SEXP verboseSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type modname(modnameSEXP);
    Rcpp::traits::input_parameter< Rcpp::DataFrame >::type data(dataSEXP);
    Rcpp::traits::input_parameter< Rcpp::List >::type MCMCParameters(MCMCParametersSEXP);
    Rcpp::traits::input_parameter< Rcpp::List >::type modelParameters(modelParametersSEXP);
    Rcpp::traits::input_parameter< int >::type nstates(nstatesSEXP);
    Rcpp::traits::input_parameter< bool >::type verbose(verboseSEXP);
    rcpp_result_gen = Rcpp::wrap(runMCMC(modname, data, MCMCParameters, modelParameters, nstates, verbose));
    return rcpp_result_gen;
END_RCPP
}
// hello_world
void hello_world();
RcppExport SEXP _bayestransmission_hello_world() {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    hello_world();
    return R_NilValue;
END_RCPP
}

RcppExport SEXP _rcpp_module_boot_Infect();

static const R_CallMethodDef CallEntries[] = {
    {"_bayestransmission_CodeToEvent", (DL_FUNC) &_bayestransmission_CodeToEvent, 1},
    {"_bayestransmission_EventToCode", (DL_FUNC) &_bayestransmission_EventToCode, 1},
    {"_bayestransmission_runMCMC", (DL_FUNC) &_bayestransmission_runMCMC, 6},
    {"_bayestransmission_hello_world", (DL_FUNC) &_bayestransmission_hello_world, 0},
    {"_rcpp_module_boot_Infect", (DL_FUNC) &_rcpp_module_boot_Infect, 0},
    {NULL, NULL, 0}
};

RcppExport void R_init_bayestransmission(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
