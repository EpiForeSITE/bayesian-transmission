// infect/SystemHistory.h
#ifndef ALUN_INFECT_SYSTEMHISTORY_H
#define ALUN_INFECT_SYSTEMHISTORY_H

#include "../util/util.h"
#include "EventCoding.h"
#include "HistoryLink.h"
#include "Model.h"
#include "System.h"
#include <map>
#include <list>

namespace infect
{
class SystemHistory: public Object, public EventCoding
{
private:



    std::map<HistoryLink*, Episode*> adm2ep;
    std::map<Episode*, HistoryLink*> ep2adm;
    std::map<Episode*, HistoryLink*> ep2dis;

    std::map<int, HistoryLink*> pheads;
    std::map<int, HistoryLink*> uheads;
    std::map<int, HistoryLink*> fheads;

    std::map<Episode*, EpisodeHistory*> ep2ephist;

	HistoryLink *shead;

	std::list<HistoryLink*> mylinks;

	HistoryLink *makeHistoryLink(Model *mod, Event *e);
	HistoryLink *makeHistoryLink(Model *mod, Facility *f, Unit *u, double t, Patient *p, EventCode c);
	int needEventType(EventCode e);

public:

	~SystemHistory();
	SystemHistory(System *s, Model *m = 0, bool verbose = 0);


	inline std::map<int, HistoryLink*>& getPatientHeads() {
	    return pheads;
	}
	inline std::map<int, HistoryLink*>& getUnitHeads() {
	    return uheads;
	}
	inline std::map<int, HistoryLink*>& getFacilityHeads() {
	    return fheads;
	}
	inline HistoryLink *getSystemHead() {
	    return shead;
	}
	inline std::map<Episode*, EpisodeHistory*>& getEpisodes() {
	    return ep2ephist;
	}
	inline std::map<Episode*, HistoryLink*>& getAdmissions() {
	    return ep2adm;
	}
	inline std::map<Episode*, HistoryLink*>& getDischarges() {
	    return ep2dis;
	}


	vector<Unit*> getUnits();

	EpisodeHistory** getPatientHistory(Patient *pat, int *n);
	List* getTestLinks();
	std::map<int, Patient*> positives();
	int sumocc();
	void write(ostream &os);
	void write2(ostream &os, int opt);
};

} // namespace infect
#endif // ALUN_INFECT_SYSTEMHISTORY_H
