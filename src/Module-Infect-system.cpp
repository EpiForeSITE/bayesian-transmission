#include "infect/infect.h"
using namespace infect;

#include "wrap.h"
#include <Rcpp.h>


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


void init_Module_infect_System(){
    using namespace Rcpp;

    class_<System>("CppSystem")
    .derives<util::Object>("CppObject")
    .constructor<std::vector<int>, std::vector<int>, std::vector<double>, std::vector<int>, std::vector<int>>()
    .property("log", &System::get_log)
    .property("start", &System::startTime)
    .property("end", &System::endTime)
    .property("Facilities", &System::getFacilities)
    .property("Units", &System::getUnits)
    .property("Patients", &System::getPatients)
    ;
    class_<infect::SystemHistory>("CppSystemHistory")
        .derives<util::Object>("CppObject")
        .constructor<infect::System*, infect::Model*, bool>()
        .property("sumocc", &infect::SystemHistory::sumocc)
        .property("UnitHeads", &infect::SystemHistory::getUnitHeads)
        .property("PatientHeads", &infect::SystemHistory::getPatientHeads)
        .property("FacilityHeads", &infect::SystemHistory::getFacilityHeads)
        .property("SystemHead", &infect::SystemHistory::getSystemHead)
        // .property("Episodes", &infect::SystemHistory::getEpisodes)
        // .property("Admissions", &infect::SystemHistory::getAdmissions)
        // .property("Discharges", &infect::SystemHistory::getDischarges)
    ;


}

