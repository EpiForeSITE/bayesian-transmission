#ifndef ALUN_INFECT_SYSTEM_H
#define ALUN_INFECT_SYSTEM_H

#include "../util/util.h"
#include "EventCoding.h"
#include "RawEventList.h"
#include "Patient.h"
#include "Episode.h"
#include <exception>
#include <map>

namespace infect
{
class lab : public Object, public EventCoding
{
public:

	int f;
	int u;
	lab(int a, int b):f(a),u(b){}

};

class System : public Object, EventCoding
{
private:

	double start;
	double end;
	IntMap *pat;
	std::map<int, Facility *> fac;
	Map *pepis;

	void handleOutOfRangeEvent(Patient *p, int t);
	void init(RawEventList *l, stringstream &err);
	void setInsitus();

protected:
    stringstream errlog;

public:

	System(RawEventList *l);
	System(RawEventList *l, stringstream &err);
	System(istream &is, stringstream &err);
	System(
	    std::vector<int> facilities,
        std::vector<int> units,
        std::vector<double> times,
        std::vector<int> patients,
        std::vector<int> types
	);
	~System();
	Map *getEpisodes(Patient *p);
	// void write(ostream &os);
	// void write2(ostream &os,int opt);

	std::string className() const override { return "System";}


	inline std::map<int, Facility*>& getFacilities() {
	    return fac;
	}


	inline List *getUnits() {
	    List *l = new List();
	    for (auto& [key, facility] : fac) {
	        for (auto& [unitKey, unit] : facility->getUnits()) {
	            l->append(unit);
	        }
	    }
	    return l;
	}


	inline IntMap *getPatients()
	{
		pat->init();
		return pat;
	}

	inline double startTime()
	{
		return start;
	}

	inline double endTime()
	{
		return end;
	}

    string get_log();
private:

	double timeOfLastKnownEvent(Episode *ep);
	Event *makeEvent(Facility *f, Unit *u, double t, Patient *p, EventCode c);
	void addEpisode(Patient *p, Episode *ep);
	RawEvent *getEvent(List *l, EventCode c, int f, int u);
	RawEvent *getEvent(List *l, int f, int u);
	bool gotAdmission(List *l, lab *x);
	void nextUnitId(List *l, int *fid, int *uid);
	void makeEvents(List *n, Patient *p, Episode **cur, Facility **f, Unit **u, stringstream &err);
	void makePatientEpisodes(List *s, stringstream &err);
	void makeAllEpisodes(RawEventList *l, stringstream &err);
	void getOrMakeFacUnit(int m, int n, Facility **f, Unit **u);
	Patient *getOrMakePatient(int n);
};

} // namespace infect
#endif // ALUN_INFECT_SYSTEM_H
