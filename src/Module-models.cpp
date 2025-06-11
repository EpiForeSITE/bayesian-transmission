
#include "modeling/modeling.h"
using namespace models;


#include "wrap.h"

#include <Rcpp.h>

#include "RRandom.h"

// Expose the classes to R
RCPP_EXPOSED_AS(util::Map)
RCPP_EXPOSED_AS(util::Random)
RCPP_EXPOSED_AS(infect::AbxLocationState)
RCPP_EXPOSED_AS(infect::AbxPatientState)
RCPP_EXPOSED_AS(infect::CountLocationState)
RCPP_EXPOSED_AS(infect::Episode)
RCPP_EXPOSED_AS(infect::EpisodeHistory)
RCPP_EXPOSED_AS(infect::Event)
RCPP_EXPOSED_AS(infect::Facility)
RCPP_EXPOSED_AS(infect::FacilityEpisodeHistory)
RCPP_EXPOSED_AS(infect::HistoryLink)
RCPP_EXPOSED_AS(infect::LocationState)
RCPP_EXPOSED_AS(infect::Model)
RCPP_EXPOSED_AS(infect::Patient)
RCPP_EXPOSED_AS(infect::PatientState)
RCPP_EXPOSED_AS(infect::RawEvent)
RCPP_EXPOSED_AS(infect::RawEventList)
RCPP_EXPOSED_AS(infect::Sampler)
RCPP_EXPOSED_AS(infect::SetLocationState)
RCPP_EXPOSED_AS(infect::System)
RCPP_EXPOSED_AS(infect::SystemEpisodeHistory)
RCPP_EXPOSED_AS(infect::SystemHistory)
RCPP_EXPOSED_AS(infect::Unit)
RCPP_EXPOSED_AS(infect::UnitEpisodeHistory)

typedef double (models::OutColParams::*OutColParams_transitionProb_iid)(int, int, double);
typedef void (models::TestParams::*TestParams_set)(int, double, int, double, double);
typedef void (models::Parameters::*update_type)(Random*);
typedef void (models::Parameters::*update_type2)(Random*, bool);

void init_Module_models(){
    class_<Parameters>("CppParameters")
        .derives<util::Object>("CppObject")
        .property("nStates", &Parameters::getNStates)
        .property("names", &Parameters::paramNames)
        .property("values", &Parameters::getValues)
        .method("logProb", &Parameters::logProb)
        .method("logProbGap", &Parameters::logProbGap)
        .method("update", (update_type)&Parameters::update)
        .method("update_max", (update_type)&Parameters::update_max)
        //.property("nParam", &Parameters::nParam)
    ;

    class_<AbxParams>("CppAbxParams")
        .derives<Parameters>("CppParameters")
        .property("nStates", &AbxParams::getNStates)
        .method("set", &AbxParams::set)
    ;

    class_<TestParams>("CppTestParams")
        .derives<Parameters>("CppParameters")
        .constructor<int>()
        .method("set", static_cast<TestParams_set>(&TestParams::set))
        .property("nParam", &TestParams::nParam)
        .property("counts", &TestParams::getCounts)//, &TestParams::setCounts)
        .method("setCount", &TestParams::setCount)
        .method("getCount", &TestParams::getCount)
    ;

    class_<InsituParams>("CppInsituParams")
        .derives<Parameters>("CppParameters")
        .constructor<>()
        .constructor<int>()
        .constructor<std::vector<double>, std::vector<double>, std::vector<bool>>()
        .property("nParam", &InsituParams::nParam)
        .property("paramNames", &InsituParams::paramNames)
        .property("values", &InsituParams::getValues)
        .property("counts", &InsituParams::getCounts, &InsituParams::setCounts)
        //     .method("set", &InsituParams::set)
        //     .method("setPriors", &InsituParams::setPriors)
        //     .method("setUpdate", &InsituParams::setUpdate)
        .method("update", &InsituParams::update)
    ;
// IncolParams
    //virtual
    class_<models::InColParams>("CppInColParams")
        .derives<Parameters>("CppParameters")
        .property("NStates", &models::InColParams::getNStates)
        .method("eventRate", &models::InColParams::eventRate)
    ;
    class_<OutColParams>("CppOutColParams")
        .derives<Parameters>("CppParameters")
        .constructor<int, int>()
        .property("NStates", &models::OutColParams::getNStates)
        .method("logProb", &models::OutColParams::logProb)
        .method("transitionProb", (OutColParams_transitionProb_iid)(&models::OutColParams::transitionProb))
    ;
    class_<models::UnitLinkedModel>("CppUnitLinkedModel")
        .derives<infect::Model>("CppModel")
        .property("InsituParams", &models::UnitLinkedModel::getInsituParams)
        .property("OutColParams", &models::UnitLinkedModel::getOutColParams)
        .property("SurveillanceTestParams", &models::UnitLinkedModel::getSurveillanceTestParams)
        .property("ClinicalTestParams", &models::UnitLinkedModel::getClinicalTestParams)
        .property("AbxParams", &models::UnitLinkedModel::getAbxParams)
        .method("logLikelihood_HL", (LogLikelihood_HL)&models::UnitLinkedModel::logLikelihood)
        .method("logLikelihood_EH", (LogLikelihood_EH)&models::UnitLinkedModel::logLikelihood)
        .method("logLikelihood_P_HL", (LogLikelihood_P_HL)&models::UnitLinkedModel::logLikelihood)
        .method("logLikelihood_P_HL_i", (LogLikelihood_P_HL_i)&models::UnitLinkedModel::logLikelihood)
        .method("logLikelihood_HL_i", (LogLikelihood_HL_i)&models::UnitLinkedModel::logLikelihood)
    ;


    class_<models::BasicModel>("CppBasicModel")
        .derives<models::UnitLinkedModel>("CppUnitLinkedModel")
        .method("forwardSimulate", &models::BasicModel::forwardSimulate)
        .method("initEpisodeHistory", &models::BasicModel::initEpisodeHistory)
        .method("sampleEpisodes", &models::BasicModel::sampleEpisodes)
    ;

    class_<models::DummyModel>("CppDummyModel")
        .derives<models::BasicModel>("CppBasicModel")
        .constructor<int>()
    ;

}
