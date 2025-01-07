#include <string>
using std::string;

#include "util/util.h"
#include "infect/infect.h"
#include "modeling/modeling.h"
#include "lognormal/lognormal.h"

#include <RcppCommon.h>


SEXP params2R(const Parameters *);
SEXP model2R(const lognormal::LogNormalModel * );

namespace Rcpp {
    template <> SEXP wrap(const lognormal::LogNormalModel& model);
    template <> SEXP wrap(const Parameters& P);
}
