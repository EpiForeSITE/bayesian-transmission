// infect/Unit.h
#ifndef ALUN_INFECT_UNIT_H
#define ALUN_INFECT_UNIT_H

#include "../util/util.h"
#include <vector>
#include <sstream>

namespace infect
{
//Forward declarations
class HistoryLink;

class Unit : public Object
{
private:

	int number;
	Object *f;

	HistoryLink* start;

public:
	Unit(Object *fac, int id)
	{
		number = id;
		f = fac;
	}

    inline HistoryLink* getStart(){return start;}
    inline void setStart(HistoryLink* s){start = s;}

    vector<HistoryLink*> getHistory(Unit* x);

	inline int getId() const
	{
		return number;
	}

	inline string getName() const
	{
		stringstream ss;
		ss << f << ":" << number;
		return ss.str();
	}

	void write(ostream &os) const override
	{
        	os << f << ":" << number;
	}
};

} // namespace infect
#endif // ALUN_INFECT_UNIT_H
