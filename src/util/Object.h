
#pragma once

#include <string.h>

#include "Allocator.h"

class Object : public Allocator
{
private:
	static long indexcounter;
	long index;

public:
	Object()
	{
		index = ++indexcounter;
	}

	virtual ~Object()
	{
	}

	virtual long hash()
	{
		return index;
	}

	virtual int compare(Object *y)
	{
		if (y == 0 || y->index < index)
			return 1;
		if (index < y->index)
			return -1;
		return 0;
	}

	virtual std::string className()
	{
		return "Object";
	}

	virtual void write(std::ostream &os)
	{
		os << className() << "[" << index << "]";
	}

	friend std::ostream &operator <<(std::ostream &os, Object *x);
};

std::ostream &operator <<(std::ostream &os, Object *x);
