# System Class Documentation - Summary

## Overview

I've added comprehensive Doxygen-style documentation to the System class in `/home/bt/src/infect/System.h`.

## Documentation Added

### Class-Level Documentation

Added a detailed class overview documenting:

1. **Purpose**: Core data structure for managing hospital system, patient movements, and events
2. **Functionality**: 
   - Event parsing and validation
   - Episode construction with temporal ordering
   - Hierarchical structure management (facilities → units → patients → episodes → events)
   - Handling missing/implicit admissions and discharges
   - Identifying "in situ" patients

3. **Data Organization**: Explanation of the three main mappings (facilities, patients, episodes)

4. **Episode Construction**: How the system intelligently handles incomplete data

5. **Event Processing**: Step-by-step description of the processing pipeline

6. **Usage Example**: Complete code example showing how to create and use a System

7. **Error Handling**: Description of logging mechanism

### Member Documentation

#### Public Members

**Constructors:**
- `System(RawEventList *l)` - From RawEventList with internal logging
- `System(RawEventList *l, stringstream &err)` - From RawEventList with custom logging
- `System(istream &is, stringstream &err)` - From input stream
- `System(std::vector<int> facilities, ...)` - **Primary R interface** - from parallel vectors

**Destructor:**
- `~System()` - Documents cleanup of all owned objects

**Public Methods:**
- `getEpisodes(Patient *p)` - Get episodes for a patient
- `getFacilities()` - Get facility map
- `getUnits()` - Get list of all units
- `getPatients()` - Get patient map
- `startTime()` - Get observation start time
- `endTime()` - Get observation end time
- `get_log()` - Get processing log

#### Private Members

**Private Data Members:**
- `start` - Start time with inline comment
- `end` - End time with inline comment
- `pat` - Patient ID map with inline comment
- `fac` - Facility ID map with inline comment
- `pepis` - Patient-episodes map with inline comment
- `errlog` - Error/warning log with inline comment

**Private Methods:**
- `handleOutOfRangeEvent()` - Handle non-standard event codes
- `init()` - Initialize from RawEventList
- `setInsitus()` - Mark initial admissions as "in situ"
- `timeOfLastKnownEvent()` - Get episode's last event time
- `makeEvent()` - Factory method for Event creation
- `addEpisode()` - Add episode to patient history
- `getEvent()` (2 overloads) - Find events in lists
- `gotAdmission()` - Check for admission events
- `nextUnitId()` - Determine next unit to process
- `makeEvents()` - Process events into episodes
- `makePatientEpisodes()` - Create all episodes for one patient
- `makeAllEpisodes()` - Create all episodes from raw data
- `getOrMakeFacUnit()` - Get or create facility/unit
- `getOrMakePatient()` - Get or create patient

### Helper Class

Also documented the `lab` helper class:
- Brief description of its purpose
- Member variable documentation (f = facility, u = unit)

## Documentation Style

- **Doxygen format**: Uses `@brief`, `@param`, `@return`, `@note`, `@see` tags
- **Inline comments**: Used `///` for brief member descriptions
- **Detailed descriptions**: Multi-paragraph explanations for complex concepts
- **Cross-references**: Added `@see` tags linking to related classes
- **Code examples**: Included usage example in class documentation
- **Parameter documentation**: Every parameter documented with type and purpose
- **Return value documentation**: Clear description of what methods return and ownership semantics

## Key Features

1. **Ownership semantics**: Clearly documented which objects are owned by System and which the caller must manage
2. **R interface**: Emphasized the vector-based constructor as the primary R interface
3. **Data requirements**: Noted that input must be sorted by patient ID, then time
4. **Error handling**: Explained the logging mechanism and how to access it
5. **Memory management**: Documented what gets deleted and when

## Benefits

- Developers can understand the System class without reading implementation
- Clear contracts for method parameters and return values
- Usage examples provide quick-start guidance
- Cross-references help navigate related classes
- Ownership documentation prevents memory leaks

## Files Modified

- `/home/bt/src/infect/System.h` - Added ~200 lines of documentation

The documentation is ready for Doxygen generation or can be viewed directly in the header file.
