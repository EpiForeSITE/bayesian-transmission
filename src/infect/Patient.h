// infect/Patient.h
#ifndef ALUN_INFECT_PATIENT_H
#define ALUN_INFECT_PATIENT_H

#include "../util/util.h"

class Patient : public Object
{
private:

	int name;

public:
	Patient(int id)
	{
		name = id;
	}

	~Patient()
	{
	}

	inline int getId()
	{
		return name;
	}

	void write(ostream &os)
	{
        	os << name;
	}
};

#endif // ALUN_INFECT_PATIENT_H
