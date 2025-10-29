#ifndef ALUN_INFECT_SYSTEM_H
#define ALUN_INFECT_SYSTEM_H

#include "../util/util.h"
#include "EventCoding.h"
#include "RawEventList.h"
#include "Patient.h"
#include "Episode.h"
#include <exception>

/**
 * @brief Helper class for facility-unit label pairs
 * 
 * Represents a (facility, unit) identifier pair used internally
 * during episode construction and event processing.
 */
class lab : public Object, public EventCoding
{
public:

	int f;  ///< Facility identifier
	int u;  ///< Unit identifier
	lab(int a, int b):f(a),u(b){}

};

/**
 * @brief Core data structure representing the hospital system
 * 
 * The System class is the central data structure for managing patient movement
 * and colonization events across hospital facilities and units. It processes
 * raw event data to construct a coherent representation of patient episodes,
 * including admissions, discharges, tests, and other events.
 * 
 * ## Overview
 * 
 * The System class performs several critical functions:
 * - Parses and validates raw event data from various sources
 * - Constructs patient episodes with proper temporal ordering
 * - Manages the hierarchical structure: facilities → units → patients → episodes → events
 * - Handles missing or implicit admissions and discharges
 * - Identifies and marks "in situ" patients (present at study start)
 * 
 * ## Data Organization
 * 
 * The system maintains three main mappings:
 * 1. **Facilities** (fac): Maps facility IDs to Facility objects containing units
 * 2. **Patients** (pat): Maps patient IDs to Patient objects
 * 3. **Patient Episodes** (pepis): Maps patients to their episode histories
 * 
 * ## Episode Construction
 * 
 * The system intelligently constructs episodes from potentially incomplete data:
 * - Creates implied admissions when events occur without recorded admission
 * - Creates implied discharges when patients move between units
 * - Adds terminal discharges at study end for patients still admitted
 * - Identifies "in situ" patients present at the start of observation
 * 
 * ## Event Processing
 * 
 * Events are processed per patient, maintaining temporal consistency:
 * 1. Sort events by patient, then time
 * 2. Group events into episodes based on unit assignments
 * 3. Add admission/discharge events as needed
 * 4. Handle special cases (transfers, out-of-range events, etc.)
 * 
 * ## Usage Example
 * 
 * ```cpp
 * // Create system from vectors of event data
 * System *sys = new System(facilities, units, times, patients, types);
 * 
 * // Access facilities and units
 * IntMap *facilities = sys->getFacilities();
 * List *units = sys->getUnits();
 * 
 * // Iterate over patients and their episodes
 * for (IntMap *patients = sys->getPatients(); patients->hasNext(); ) {
 *     Patient *p = (Patient*) patients->nextValue();
 *     Map *episodes = sys->getEpisodes(p);
 *     for (episodes->init(); episodes->hasNext(); ) {
 *         Episode *ep = (Episode*) episodes->next();
 *         // Process episode...
 *     }
 * }
 * 
 * // Get study time bounds
 * double start = sys->startTime();
 * double end = sys->endTime();
 * ```
 * 
 * ## Error Handling
 * 
 * The system logs warnings and errors to an internal stringstream:
 * - Missing admissions/discharges that were added
 * - Out-of-range event codes
 * - Data inconsistencies
 * 
 * Access the log with `get_log()` to review processing issues.
 * 
 * @note The System class owns all created Patient, Facility, Unit, Episode, 
 *       and Event objects. They are destroyed in the destructor.
 * @note Event data must be sorted by patient ID, then time before construction.
 *       The runMCMC function validates this requirement.
 * 
 * @see Patient, Episode, Event, Facility, Unit
 * @see RawEventList for input data format
 * @see SystemHistory for model-based analysis of the system
 */
class System : public Object, EventCoding
{
private:

	double start;
	double end;
	IntMap *pat;
	IntMap *fac;
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

	inline IntMap *getFacilities()
	{
		fac->init();
		return fac;
	}

	inline List *getUnits()
	{
		List *l = new List();

		for (fac->init(); fac->hasNext(); )
			for (IntMap *u = ((Facility *)fac->nextValue())->getUnits(); u->hasNext(); )
				l->append(u->nextValue());

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

class System : public Object, EventCoding
{
private:

	double start;  ///< Start time of observation period (time of first event)
	double end;    ///< End time of observation period (time of last event + epsilon)
	IntMap *pat;   ///< Map from patient ID to Patient objects
	IntMap *fac;   ///< Map from facility ID to Facility objects
	Map *pepis;    ///< Map from Patient to Map of Episodes

	/**
	 * @brief Handle events with out-of-range type codes
	 * 
	 * Events with type codes outside the standard range are interpreted
	 * as patient group assignments.
	 * 
	 * @param p Patient receiving the group assignment
	 * @param t Type code (interpreted as group ID)
	 */
	void handleOutOfRangeEvent(Patient *p, int t);
	
	/**
	 * @brief Initialize the system from a RawEventList
	 * 
	 * @param l Raw event list to process
	 * @param err Error/warning log stream
	 */
	void init(RawEventList *l, stringstream &err);
	
	/**
	 * @brief Mark initial admissions as "in situ"
	 * 
	 * Identifies patients present at the start of the observation period
	 * and converts their admission events to "insitu" variants (insitu,
	 * insitu0, insitu1, insitu2) to indicate they were already present.
	 */
	void setInsitus();

protected:
    stringstream errlog;  ///< Log of warnings and errors during processing

public:

	/**
	 * @brief Construct System from RawEventList
	 * 
	 * Processes a RawEventList to build the complete system structure.
	 * Logs warnings/errors to internal errlog.
	 * 
	 * @param l RawEventList containing all events to process
	 */
	System(RawEventList *l);
	
	/**
	 * @brief Construct System from RawEventList with custom error log
	 * 
	 * @param l RawEventList containing all events to process
	 * @param err Custom stringstream for logging warnings/errors
	 */
	System(RawEventList *l, stringstream &err);
	
	/**
	 * @brief Construct System from input stream with custom error log
	 * 
	 * @param is Input stream containing raw event data
	 * @param err Custom stringstream for logging warnings/errors
	 */
	System(istream &is, stringstream &err);
	
	/**
	 * @brief Construct System from vectors of event data (R interface)
	 * 
	 * This constructor is the primary interface from R, accepting parallel
	 * vectors representing the event data. Events must be pre-sorted by
	 * patient ID, then time.
	 * 
	 * @param facilities Vector of facility IDs for each event
	 * @param units Vector of unit IDs for each event
	 * @param times Vector of event times (double precision)
	 * @param patients Vector of patient IDs for each event
	 * @param types Vector of event type codes (see EventCoding)
	 * 
	 * @note Vectors must all have the same length
	 * @note Data must be sorted by patient ID, then time
	 * @throws Runtime error if data is not properly sorted
	 */
	System(
	    std::vector<int> facilities,
        std::vector<int> units,
        std::vector<double> times,
        std::vector<int> patients,
        std::vector<int> types
	);
	
	/**
	 * @brief Destructor - cleans up all owned objects
	 * 
	 * Deletes all Patient, Facility, Unit, Episode, and Event objects
	 * created during system construction.
	 */
	~System();
	
	/**
	 * @brief Get all episodes for a given patient
	 * 
	 * Returns a Map of episodes sorted chronologically. The map iterator
	 * is initialized before returning.
	 * 
	 * @param p Patient whose episodes to retrieve
	 * @return Map* Pointer to map of Episode objects (caller should not delete)
	 */
	Map *getEpisodes(Patient *p);
	
	// void write(ostream &os);
	// void write2(ostream &os,int opt);

	std::string className() const override { return "System";}

	/**
	 * @brief Get map of all facilities in the system
	 * 
	 * Returns an IntMap mapping facility IDs to Facility objects.
	 * The map iterator is initialized before returning.
	 * 
	 * @return IntMap* Pointer to facility map (caller should not delete)
	 */
	inline IntMap *getFacilities()
	{
		fac->init();
		return fac;
	}

	/**
	 * @brief Get list of all units across all facilities
	 * 
	 * Constructs and returns a new List containing Unit pointers from
	 * all facilities in the system.
	 * 
	 * @return List* Pointer to new list of Unit objects (caller must delete)
	 */
	inline List *getUnits()
	{
		List *l = new List();

		for (fac->init(); fac->hasNext(); )
			for (IntMap *u = ((Facility *)fac->nextValue())->getUnits(); u->hasNext(); )
				l->append(u->nextValue());

		return l;
	}

	/**
	 * @brief Get map of all patients in the system
	 * 
	 * Returns an IntMap mapping patient IDs to Patient objects.
	 * The map iterator is initialized before returning.
	 * 
	 * @return IntMap* Pointer to patient map (caller should not delete)
	 */
	inline IntMap *getPatients()
	{
		pat->init();
		return pat;
	}

	/**
	 * @brief Get start time of observation period
	 * 
	 * @return double Time of the first event in the system
	 */
	inline double startTime()
	{
		return start;
	}

	/**
	 * @brief Get end time of observation period
	 * 
	 * @return double Time slightly after the last event (rounded up + epsilon)
	 */
	inline double endTime()
	{
		return end;
	}

    /**
     * @brief Get the processing log as a string
     * 
     * Returns all warnings and errors encountered during system construction,
     * including information about added admissions/discharges and data issues.
     * 
     * @return string Log contents
     */
    string get_log();
    
private:

	/**
	 * @brief Get time of the last event in an episode
	 * 
	 * @param ep Episode to examine
	 * @return double Time of last event (admission or internal event)
	 */
	double timeOfLastKnownEvent(Episode *ep);
	
	/**
	 * @brief Create a new Event object
	 * 
	 * Factory method for creating events with proper initialization.
	 * 
	 * @param f Facility where event occurs
	 * @param u Unit where event occurs
	 * @param t Time of event
	 * @param p Patient experiencing event
	 * @param c Event type code
	 * @return Event* Newly created event (caller should not delete directly)
	 */
	Event *makeEvent(Facility *f, Unit *u, double t, Patient *p, EventCode c);
	
	/**
	 * @brief Add an episode to a patient's history
	 * 
	 * Creates the patient's episode map if it doesn't exist.
	 * 
	 * @param p Patient to add episode to
	 * @param ep Episode to add
	 */
	void addEpisode(Patient *p, Episode *ep);
	
	/**
	 * @brief Find a specific event type in a list
	 * 
	 * @param l List of RawEvent objects to search
	 * @param c Event type code to find
	 * @param f Facility ID to match (0 = any)
	 * @param u Unit ID to match (0 = any)
	 * @return RawEvent* Found event or nullptr
	 */
	RawEvent *getEvent(List *l, EventCode c, int f, int u);
	
	/**
	 * @brief Find any non-admission/discharge event in a list
	 * 
	 * @param l List of RawEvent objects to search
	 * @param f Facility ID to match (0 = any)
	 * @param u Unit ID to match (0 = any)
	 * @return RawEvent* Found event or nullptr
	 */
	RawEvent *getEvent(List *l, int f, int u);
	
	/**
	 * @brief Check if list contains an admission event for a location
	 * 
	 * @param l List of RawEvent objects to search
	 * @param x Location (facility, unit pair) to check
	 * @return bool True if admission found
	 */
	bool gotAdmission(List *l, lab *x);
	
	/**
	 * @brief Determine next unit to process from event list
	 * 
	 * Uses heuristics to determine which facility/unit should be processed
	 * next based on discharge events, admissions, and other factors.
	 * 
	 * @param l List of RawEvent objects
	 * @param fid Output: selected facility ID
	 * @param uid Output: selected unit ID
	 */
	void nextUnitId(List *l, int *fid, int *uid);
	
	/**
	 * @brief Process a list of events into episodes for a patient
	 * 
	 * Main processing logic that creates episodes with proper admission/discharge
	 * events, adding implied events as needed.
	 * 
	 * @param n List of RawEvent objects to process
	 * @param p Patient whose events are being processed
	 * @param cur Pointer to current episode (updated during processing)
	 * @param f Pointer to current facility (updated during processing)
	 * @param u Pointer to current unit (updated during processing)
	 * @param err Error/warning log stream
	 */
	void makeEvents(List *n, Patient *p, Episode **cur, Facility **f, Unit **u, stringstream &err);
	
	/**
	 * @brief Create all episodes for a single patient
	 * 
	 * @param s List of RawEvent objects for one patient
	 * @param err Error/warning log stream
	 */
	void makePatientEpisodes(List *s, stringstream &err);
	
	/**
	 * @brief Create episodes for all patients from raw event list
	 * 
	 * Top-level processing function that groups events by patient and
	 * calls makePatientEpisodes for each.
	 * 
	 * @param l RawEventList containing all events
	 * @param err Error/warning log stream
	 */
	void makeAllEpisodes(RawEventList *l, stringstream &err);
	
	/**
	 * @brief Get or create Facility and Unit objects
	 * 
	 * Looks up or creates the specified facility and unit, updating
	 * the facility's unit map as needed.
	 * 
	 * @param m Facility ID
	 * @param n Unit ID
	 * @param f Output: pointer to Facility object
	 * @param u Output: pointer to Unit object
	 */
	void getOrMakeFacUnit(int m, int n, Facility **f, Unit **u);
	
	/**
	 * @brief Get or create a Patient object
	 * 
	 * @param n Patient ID
	 * @return Patient* Pointer to existing or newly created Patient
	 */
	Patient *getOrMakePatient(int n);
};

#endif // ALUN_INFECT_SYSTEM_H
