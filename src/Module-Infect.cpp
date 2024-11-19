#include "infect/infect.h"
using namespace infect;

#include <Rcpp.h>
using namespace Rcpp;

RCPP_MODULE(Infect){

    class_<RawEventList>("RawEventList")
    .constructor<
        std::vector<int>,    // facilities
        std::vector<int>,    // units
        std::vector<double>, // times
        std::vector<int>,    // patients
        std::vector<int>     // types
    >()
    .method("FirstTime", &RawEventList::firstTime)
    .method("LastTime", &RawEventList::lastTime)
    ;
}
