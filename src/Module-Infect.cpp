#include "infect/infect.h"
using namespace infect;

#include "wrap.h"
#include <Rcpp.h>

#include "RRandom.h"

// Expose the classes to R
RCPP_EXPOSED_AS(RRandom)

RCPP_EXPOSED_CLASS_NODECL(util::Object)
RCPP_EXPOSED_CLASS_NODECL(util::MapLink)

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

void init_Module_infect_System();
void init_Module_infect(){
    using namespace Rcpp;

    class_<AbxLocationState>("CppAbxLocationState")
        .derives<util::Object>("CppObject")
        //.derives<CountLocationState>("CppCountLocationState")
        .property("nStates"    , static_cast<int (infect::AbxLocationState::*)() const>(&infect::LocationState::nStates))
        .property("Total"      , static_cast<int (infect::AbxLocationState::*)() const>(&infect::CountLocationState::getTotal))
        .property("Colonized"  , static_cast<int (infect::AbxLocationState::*)() const>(&infect::CountLocationState::getColonized))
        .property("Latent"     , static_cast<int (infect::AbxLocationState::*)() const>(&infect::CountLocationState::getLatent))
        .property("Susceptible", static_cast<int (infect::AbxLocationState::*)() const>(&infect::CountLocationState::getSusceptible))
        .method("onAbx", &infect::AbxLocationState::onAbx)
        .method("everAbx", &infect::AbxLocationState::everAbx)
        .property("AbxTotal", &infect::AbxLocationState::getAbxTotal)
        .property("EverAbxTotal", &infect::AbxLocationState::getEverAbxTotal)
        .property("AbxColonized", &infect::AbxLocationState::getAbxColonized)
        .property("EverAbxColonized", &infect::AbxLocationState::getEverAbxColonized)
        .property("AbxLatent", &infect::AbxLocationState::getAbxLatent)
        .property("EverAbxLatent", &infect::AbxLocationState::getEverAbxLatent)
        .property("AbxSusceptible", &infect::AbxLocationState::getAbxSusceptible)
        .property("EverAbxSusceptible", &infect::AbxLocationState::getEverAbxSusceptible)
        .property("NoAbxTotal", &infect::AbxLocationState::getNoAbxTotal)
        .property("NeverAbxTotal", &infect::AbxLocationState::getNeverAbxTotal)
        .property("NoAbxColonized", &infect::AbxLocationState::getNoAbxColonized)
        .property("NeverAbxColonized", &infect::AbxLocationState::getNeverAbxColonized)
        .property("NoAbxLatent", &infect::AbxLocationState::getNoAbxLatent)
        .property("NeverAbxLatent", &infect::AbxLocationState::getNeverAbxLatent)
        .property("NoAbxSusceptible", &infect::AbxLocationState::getNoAbxSusceptible)
        .property("NeverAbxSusceptible", &infect::AbxLocationState::getNeverAbxSusceptible)
    ;
    class_<infect::AbxPatientState>("CppAbxPatientState")
        // .derives<PatientState>("CppPatientState")
        .derives<util::Object>("CppObject")
        // .constructor<Patient*>()
        // .constructor<Patient*, int>()
        .property("unit", static_cast<infect::Unit* (infect::AbxPatientState::*)() const>(&infect::PatientState::getUnit))
        .property("infectionStatus", static_cast<infect::InfectionCoding::InfectionStatus (infect::AbxPatientState::*)() const>(&infect::PatientState::infectionStatus))
        .property("onAbx", &infect::AbxPatientState::onAbx)
        .property("everAbx", &infect::AbxPatientState::everAbx)
    ;
    class_<CountLocationState>("CppCountLocationState")
        // .derives<LocationState>("CppLocationState")
        .derives<util::Object>("CppObject")
        .property("nStates", static_cast<int (infect::CountLocationState::*)() const>(&infect::CountLocationState::nStates))
        .property("Total", &infect::CountLocationState::getTotal)
        .property("Colonized", &infect::CountLocationState::getColonized)
        .property("Latent", &infect::CountLocationState::getLatent)
        .property("Susceptible", &infect::CountLocationState::getSusceptible)
    ;
    class_<Event>("CppEvent")
        .derives<util::Object>("CppObject")
        .constructor()
        .property("time", &Event::getTime)
        .property("type", &Event::getTypeAsString)
        .property("type_int", &Event::getTypeAsInt)
        .property("patient", &Event::getPatient)
        .property("facility", &Event::getFacility)
        .property("unit", &Event::getUnit)
        .property("isTest", &Event::isTest)
        .property("isPositiveTest", &Event::isPositiveTest)
        .property("isClinicalTest", &Event::isClinicalTest)
        .property("isAdmission", &Event::isAdmission)
    ;
    class_<infect::Facility>("CppFacility")
        .derives<util::Object>("CppObject")
        .constructor<int>()
        .property("id", &infect::Facility::getId)
        .method("getUnit", &infect::Facility::getUnit)
        // .method("getUnits", &infect::Facility::getUnits)
    ;
    class_<infect::HistoryLink>("CppHistoryLink")
        .derives<util::Object>("CppObject")
        // .constructor()
        .property("linked", &HistoryLink::isLinked, &infect::HistoryLink::setLinked)
        .property("hidden", &HistoryLink::isHidden, &infect::HistoryLink::setHidden)
        .property("PatientPrev", &HistoryLink::pPrev)
        .property("PatientNext", &HistoryLink::pNext)
        .property("HistoryPrev", &HistoryLink::hPrev)
        .property("HistoryNext", &HistoryLink::hNext)
        .property("UnitPrev", &infect::HistoryLink::uPrev)
        .property("UnitNext", &HistoryLink::uNext)
        .property("FacilityPrev", &HistoryLink::fPrev)
        .property("FacilityNext", &HistoryLink::fNext)
        .property("SystemPrev", &HistoryLink::sPrev)
        .property("SystemNext", &HistoryLink::sNext)
        .property("Event", &HistoryLink::getEvent)
        .property("PatientState", &HistoryLink::getPState)
        .property("UnitState", &HistoryLink::getUState)
        .property("FacilityState", &HistoryLink::getFState)
        .property("SystemState", &HistoryLink::getSState)
    ;
    class_<LocationState>("CppLocationState")
        .derives<util::Object>("CppObject")
        // .derives<State>("CppState")
        // .constructor<Object*, int>()
        .property("Total", &infect::LocationState::getTotal)
        .property("Colonized", &infect::LocationState::getColonized)
        .property("Latent", &infect::LocationState::getLatent)
        .property("Susceptible", &infect::LocationState::getSusceptible)
        .property("nStates", &infect::LocationState::nStates)
    ;
    class_<infect::Model>("CppModel")
        .derives<util::Object>("CppObject")
        .property("AbxLife", &infect::Model::getAbxLife, &infect::Model::setAbxLife)
        .property("AbxDelay", &infect::Model::getAbxDelay, &infect::Model::setAbxDelay)

        .property("cheating", &infect::Model::isCheating)
        // .property("nStates", &infect::Model::getNStates)
        // .property("nUnits", &infect::Model::getNUnits)
        // .property("nPatients", &infect::Model::getNPatients)
        // .property("nFacilities", &infect::Model::getNFacilities)

        .method("logLikelihood", &infect::Model::logLikelihood)
        .method("update", &infect::Model::update)
        .method("forwardSimulate", &infect::Model::forwardSimulate)
        .method("initEpisodeHistory", &infect::Model::initEpisodeHistory)
        .method("sampleEpisodes", &infect::Model::sampleEpisodes)
        .method("makeEpisodeHistory", &infect::Model::makeEpisodeHistory)
    ;
    class_<infect::Patient>("CppPatient")
        .derives<util::Object>("CppObject")
        .constructor<int>()
        .property("id", &Patient::getId)
        .property("group", &Patient::getGroup, &Patient::setGroup)
    ;
    class_<infect::PatientState>("CppPatientState")
        .derives<util::Object>("CppObject")
        .constructor<Patient*>()
        .constructor<Patient*, int>()
        .property("unit", &infect::PatientState::getUnit)
        .property("infectionStatus", &infect::PatientState::infectionStatus)
        .property("onAbx", &infect::PatientState::onAbx)
    ;
    class_<RawEventList>("CppRawEventList")
        .derives<util::Object>("CppObject")
        .constructor<
            std::vector<int>,    // facilities
            std::vector<int>,    // units
            std::vector<double>, // times
            std::vector<int>,    // patients
            std::vector<int>     // types
        >()
        .method("FirstTime", &RawEventList::firstTime)
        .method("LastTime", &RawEventList::lastTime)
        .method("getEvents", &RawEventList::getEvents)
    ;
    class_<infect::Sampler>("CppSampler")
        .derives<util::Object>("CppObject")
        .constructor<SystemHistory*, Model*, Random*>()
        .method<void>("sampleModel", &infect::Sampler::sampleModel)
        .method<void>("sampleEpisodes", &infect::Sampler::sampleEpisodes)
    ;
    class_<TestParamsAbx>("CppTestParamsAbx")
        .derives<util::Object>("CppObject")
        //     .constructor<int, bool>()
        //     .property("paramNames", &TestParamsAbx::paramNames)
        //     .property("UseAbx", &TestParamsAbx::getUseAbx, &TestParamsAbx::setUseAbx)
        //     .property("nParam", &TestParamsAbx::nParam)
        //     .method<void, int, int, double, int, double, double>("set", &TestParamsAbx::set)
        //     // .method("update", &TestParamsAbx::update)
    ;
    class_<infect::Unit>("CppUnit")
        .derives<util::Object>("CppObject")
        .constructor<Object*, int>()
        .property("id", &infect::Unit::getId)
        .method("getName", &infect::Unit::getName)
        .method("getHistory", &infect::Unit::getHistory)
    ;

    class_<infect::RawEvent>("CppRawEvent")
        .derives<util::Object>("CppObject")
        .constructor<int, int, double, int, int>()
        .property("facility", &infect::RawEvent::getFacilityId)
        .property("unit", &infect::RawEvent::getUnitId)
        .property("time", &infect::RawEvent::getTime)
        .property("patient", &infect::RawEvent::getPatientId)
        .property("type", &infect::RawEvent::getTypeId)
    ;

    init_Module_infect_System();
}
