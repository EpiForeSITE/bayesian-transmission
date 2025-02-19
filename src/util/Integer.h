#ifndef ALUN_UTIL_INTEGER_H
#define ALUN_UTIL_INTEGER_H
#include "Object.h"

class Integer : public Object
{
public:
	int value;

	Integer(int x) : Object()
	{
		value = x;
	}

	int intValue() const
	{
		return value;
	}

	void set(int n)
	{
		value = n;
	}

	std::string className() const
	{
		return "Integer";
	}

	void write(std::ostream &os) const
	{
		os << value;
	}
};
#endif // ALUN_UTIL_INTEGER_H
