#ifndef ALUN_INFECT_FACILITY_H
#define ALUN_INFECT_FACILITY_H

#include "../util/util.h"
#include "Unit.h"
#include <map>

namespace infect
{
class Facility : public Object {
private:
    int number;
    std::map<int, Unit*> unit; // Changed from IntMap to std::map
public:
    Facility(int id) : number(id) {}
    ~Facility();

    void write(ostream &os) const;

    inline void addUnit(Unit *u) {
        unit[u->getId()] = u;
    }

    inline Unit *getUnit(int i) const {
        auto it = unit.find(i);
        return it != unit.end() ? it->second : nullptr;
    }

    inline std::map<int, Unit*>& getUnits() {
        return unit;
    }

    inline int getId() const {
        return number;
    }
};
} // namespace infect
#endif // ALUN_INFECT_FACILITY_H
