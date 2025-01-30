#ifndef bayesian_transmission_wrap_h
#define bayesian_transmission_wrap_h

#include <string>
using std::string;

#include "util/util.h"
#include "infect/infect.h"
#include "modeling/modeling.h"
#include "lognormal/lognormal.h"

#include <RcppCommon.h>


SEXP params2R(const Parameters *);
SEXP model2R(const lognormal::LogNormalModel * );
// SEXP HistoryLink2R(const infect::HistoryLink *);


namespace Rcpp {
    template <> SEXP wrap(const lognormal::LogNormalModel& model);
    template <> SEXP wrap(const Parameters& P);
}
#endif // bayesian_transmission_wrap_h
