#include "infect/infect.h"
using namespace infect;

#include "lognormal/lognormal.h"
using namespace lognormal;

#include <Rcpp.h>
using namespace Rcpp;

#include "RRandom.h"
#include <functional>

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

    class_<System>("CppTransmissionSystem")
        .constructor<std::vector<int>, std::vector<int>, std::vector<double>, std::vector<int>, std::vector<int>>()
        .property("log", &System::get_log)
        .property("start", &System::startTime)
        .property("end", &System::endTime)
    ;

    class_<RRandom>("RRandom")
        .constructor<>()
        .method("runif", (double(RRandom::*)()) &RRandom::runif)
        .method<double, double, double>("runif2", (double(RRandom::*)(double, double)) &RRandom::runif)
        .method("rexp", (double(RRandom::*)()) &RRandom::rexp)
        .method("rexp1", (double(RRandom::*)(double)) &RRandom::rexp)
        .method("rgamma", &RRandom::rgamma)
        .method("rnorm", (double(RRandom::*)()) &RRandom::rnorm)
        .method("rnorm2", (double(RRandom::*)(double, double))&RRandom::rnorm)
        .method("rpoisson", &RRandom::rpoisson)
        //.method("", &RRandom::)
    ;

    class_<InsituParams>("CppInsituParams")
        .constructor<int>()
        .constructor<std::vector<double>, std::vector<double>, std::vector<bool>>()
        .property("nParam", &InsituParams::nParam)
        .property("paramNames", &InsituParams::paramNames)
        .method("set", &InsituParams::set)
        .method("setPriors", &InsituParams::setPriors)
        .method("setUpdate", &InsituParams::setUpdate)
        //.method("update", &InsituParams::update)
    ;

    class_<TestParamsAbx>("CppTestParamsAbx")
        .constructor<int, bool>()
        .property("paramNames", &TestParamsAbx::paramNames)
        .property("UseAbx", &TestParamsAbx::getUseAbx, &TestParamsAbx::setUseAbx)
        .property("nParam", &TestParamsAbx::nParam)
        .method<void, int, int, double, int, double, double>("set", &TestParamsAbx::set)
        // .method("update", &TestParamsAbx::update)
    ;

    class_<LogNormalModel>("CppLogNormalModel")
        .property<double>("AbxLife", &infect::Model::getAbxLife, &infect::Model::setAbxLife)
        .property<double>("AbxDelay", &infect::Model::getAbxDelay, &infect::Model::setAbxDelay)
        .property<bool>("isCheating", &infect::Model::isCheating)
    ;

    class_<TestParams>("CppTestParams")
        .constructor<int>()
        .property("paramNames", &TestParams::paramNames)
        .property("nParam", &TestParams::nParam)
        .property("values", &TestParams::getValues)
        // .method("set", &TestParams::set)
        // .method("update", &TestParams::update)
    ;
}
