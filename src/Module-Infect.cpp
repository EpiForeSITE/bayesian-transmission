#include "infect/infect.h"
using namespace infect;

#include "wrap.h"
#include <Rcpp.h>
#include <map>
#include <utility>

#include "RRandom.h"

// Expose the classes to R
RCPP_EXPOSED_AS(RRandom)

RCPP_EXPOSED_CLASS_NODECL(util::Object)
RCPP_EXPOSED_CLASS_NODECL(util::MapLink)

RCPP_EXPOSED_AS(util::IntMap)
RCPP_EXPOSED_AS(util::List)
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

// Global cache to keep shared_ptrs alive as long as System exists
// This prevents the IntMap/Map from being deleted when returned to R
static std::map<infect::System*, std::shared_ptr<util::IntMap>> fac_cache;
static std::map<infect::System*, std::shared_ptr<util::IntMap>> pat_cache;
static std::map<std::pair<infect::System*, infect::Patient*>, std::shared_ptr<util::Map>> eps_cache;

// Wrapper functions that explicitly wrap returned pointers in reference classes
// with XPtr(p, false) to prevent R from deleting them (System owns them via shared_ptr)
// The shared_ptr is also cached to keep the container alive even if intermediate R objects are GC'd

static SEXP System_getFacilities_wrapper(infect::System* sys) {
    auto sp = sys->getFacilities();
    fac_cache[sys] = sp;  // Keep shared_ptr alive
    util::IntMap* p = sp.get();
    if (p == nullptr) {
        return R_NilValue;
    }
    Rcpp::Function methods_new = Rcpp::Environment::namespace_env("methods")["new"];
    return methods_new("Rcpp_CppIntMap", 
                      Rcpp::Named(".object_pointer") = Rcpp::XPtr<util::IntMap>(p, false));
}

static SEXP System_getPatients_wrapper(infect::System* sys) {
    auto sp = sys->getPatients();
    pat_cache[sys] = sp;  // Keep shared_ptr alive
    util::IntMap* p = sp.get();
    if (p == nullptr) {
        return R_NilValue;
    }
    Rcpp::Function methods_new = Rcpp::Environment::namespace_env("methods")["new"];
    return methods_new("Rcpp_CppIntMap", 
                      Rcpp::Named(".object_pointer") = Rcpp::XPtr<util::IntMap>(p, false));
}

static SEXP System_getEpisodes_wrapper(infect::System* sys, infect::Patient* p) {
    auto sp = sys->getEpisodes(p);
    eps_cache[std::make_pair(sys, p)] = sp;  // Keep shared_ptr alive
    util::Map* m = sp.get();
    if (m == nullptr) {
        return R_NilValue;
    }
    Rcpp::Function methods_new = Rcpp::Environment::namespace_env("methods")["new"];
    return methods_new("Rcpp_CppMap", 
                      Rcpp::Named(".object_pointer") = Rcpp::XPtr<util::Map>(m, false));
}

/**
 * @brief Wrapper function to extract all Events from a SystemHistory and return them as an Rcpp::List
 * 
 * This function traverses the SystemHistory via the system history chain (sNext links)
 * and collects all Event objects, wrapping each in its corresponding Rcpp reference class.
 * The Events are returned in chronological order as they appear in the history chain.
 * 
 * @param hist Pointer to the SystemHistory object to extract events from
 * @return SEXP An Rcpp::List containing wrapped CppEvent reference class objects.
 *              Returns empty list if hist is nullptr or has no events.
 *              Each Event pointer is wrapped with XPtr(event, false) to indicate
 *              that R should NOT delete these objects (SystemHistory owns them).
 * 
 * @note The returned Event objects share memory with the SystemHistory and should
 *       not be modified or deleted from R. They are read-only references.
 * 
 * @usage In R: event_list <- system_history$getEventList()
 */
static SEXP SystemHistory_getEventList_wrapper(infect::SystemHistory* hist) {
    if (hist == nullptr) {
        return R_NilValue;
    }
    
    Rcpp::List event_list;
    Rcpp::Function methods_new = Rcpp::Environment::namespace_env("methods")["new"];
    
    // Get the system head and traverse all history links
    infect::HistoryLink* link = hist->getSystemHead();
    
    if (link == nullptr) {
        return event_list;  // Return empty list
    }
    
    // Traverse the system history chain via sNext() to collect all events
    for (; link != nullptr; link = link->sNext()) {
        infect::Event* event = link->getEvent();
        
        if (event != nullptr) {
            // Wrap the Event pointer in an Rcpp reference class
            // Use XPtr with false flag to indicate R should not delete it
            // (the SystemHistory owns these events)
            SEXP event_obj = methods_new("Rcpp_CppEvent",
                                        Rcpp::Named(".object_pointer") = Rcpp::XPtr<infect::Event>(event, false));
            event_list.push_back(event_obj);
        }
    }
    
    return event_list;
}

/**
 * @brief Wrapper function to extract all HistoryLinks from a SystemHistory and return them as an Rcpp::List
 * 
 * This function traverses the SystemHistory via the system history chain (sNext links)
 * and collects all HistoryLink objects, wrapping each in its corresponding Rcpp reference class.
 * 
 * @param hist Pointer to the SystemHistory object to extract history links from
 * @return SEXP An Rcpp::List containing wrapped CppHistoryLink reference class objects.
 *              Returns empty list if hist is nullptr or has no history links.
 * 
 * @note The returned HistoryLink objects share memory with the SystemHistory and should
 *       not be modified or deleted from R. They are read-only references.
 * 
 * @usage In R: link_list <- system_history$getHistoryLinkList()
 */
static SEXP SystemHistory_getHistoryLinkList_wrapper(infect::SystemHistory* hist) {
    if (hist == nullptr) {
        return R_NilValue;
    }
    
    Rcpp::List link_list;
    Rcpp::Function methods_new = Rcpp::Environment::namespace_env("methods")["new"];
    
    // Get the system head and traverse all history links
    infect::HistoryLink* link = hist->getSystemHead();
    
    if (link == nullptr) {
        return link_list;  // Return empty list
    }
    
    // Traverse the system history chain via sNext() to collect all history links
    for (; link != nullptr; link = link->sNext()) {
        // Wrap the HistoryLink pointer in an Rcpp reference class
        // Use XPtr with false flag to indicate R should not delete it
        SEXP link_obj = methods_new("Rcpp_CppHistoryLink",
                                   Rcpp::Named(".object_pointer") = Rcpp::XPtr<infect::HistoryLink>(link, false));
        link_list.push_back(link_obj);
    }
    
    return link_list;
}

/**
 * @brief Wrapper to compute log likelihood for a single HistoryLink
 * 
 * @param model Pointer to the Model (must be castable to UnitLinkedModel)
 * @param link Pointer to the HistoryLink to compute likelihood for
 * @return double The log likelihood contribution of this link (including gap probability)
 */
static double Model_logLikelihoodLink_wrapper(infect::Model* model, infect::HistoryLink* link) {
    if (model == nullptr || link == nullptr) {
        return 0.0;
    }
    
    // Cast to UnitLinkedModel to access the HistoryLink-specific logLikelihood method
    UnitLinkedModel* ulm = dynamic_cast<UnitLinkedModel*>(model);
    if (ulm == nullptr) {
        Rcpp::stop("Model must be a UnitLinkedModel to compute likelihood for individual links");
    }
    
    return ulm->logLikelihood(link);
}

// TODO: Add System destructor hook to clear cache entries for deleted Systems

void init_Module_infect(){
    using namespace Rcpp;

// getUnits() and getSystemCounts() create new objects, so R should delete them
// Keep default behavior for those

    class_<infect::AbxLocationState>("CppAbxLocationState")
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
        .property("Time", &Event::getTime)
        .property("Type", &Event::getTypeAsString)
        .property("Patient", &Event::getPatient)
        .property("Facility", &Event::getFacility)
        .property("Unit", &Event::getUnit)
        .property("isTest", &Event::isTest)
        .property("isPositiveTest", &Event::isPositiveTest)
        .property("isClinicalTest", &Event::isClinicalTest)
        .property("isAdmission", &Event::isAdmission)
    ;
    class_<Episode>("CppEpisode")
        .derives<util::Object>("CppObject")
        .constructor()
        .property("Admission", &Episode::getAdmission)
        .property("Discharge", &Episode::getDischarge)
        .method("getEvents", &Episode::getEvents)
        .method("hasAdmission", &Episode::hasAdmission)
        .method("hasDischarge", &Episode::hasDischarge)
        .method("hasEvents", &Episode::hasEvents)
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
        .method("logLikelihoodLink", &Model_logLikelihoodLink_wrapper)
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
        .derives<util::SortedList>("CppSortedList")
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
    class_<infect::Sampler>("CppSampler")
        .derives<util::Object>("CppObject")
        .constructor<SystemHistory*, Model*, Random*>()
        .method<void>("sampleModel", &infect::Sampler::sampleModel)
        .method<void>("sampleEpisodes", &infect::Sampler::sampleEpisodes)
    ;
    class_<System>("CppSystem")
        .derives<util::Object>("CppObject")
        .constructor<std::vector<int>, std::vector<int>, std::vector<double>, std::vector<int>, std::vector<int>>()
        .property("log", &System::get_log)
        .method("getFacilities", &System_getFacilities_wrapper)
        .method("getUnits", &System::getUnits)
        .method("getPatients", &System_getPatients_wrapper)
        .method("getEpisodes", &System_getEpisodes_wrapper)
        .method("startTime", &System::startTime)
        .method("endTime", &System::endTime)
        .method("countEpisodes", (int (infect::System::*)() const)&infect::System::countEpisodes)
        .method("countEvents", (int (infect::System::*)() const)&infect::System::countEvents)
        .method("getSystemCounts", (util::List* (infect::System::*)() const)&infect::System::getSystemCounts)
    ;
    class_<infect::SystemHistory>("CppSystemHistory")
        .derives<util::Object>("CppObject")
        .constructor<infect::System*, infect::Model*, bool>()
        .property("sumocc", &infect::SystemHistory::sumocc)
        .property("UnitHeads", &infect::SystemHistory::getUnitHeads)
        .property("PatientHeads", &infect::SystemHistory::getPatientHeads)
        .property("FacilityHeads", &infect::SystemHistory::getFacilityHeads)
        .property("SystemHead", &infect::SystemHistory::getSystemHead)
        .property("Episodes", &infect::SystemHistory::getEpisodes)
        .property("Admissions", &infect::SystemHistory::getAdmissions)
        .property("Discharges", &infect::SystemHistory::getDischarges)
        .method("getEventList", &SystemHistory_getEventList_wrapper)
        .method("getHistoryLinkList", &SystemHistory_getHistoryLinkList_wrapper)
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
        .method("getName", &infect::Unit::getName);


}
