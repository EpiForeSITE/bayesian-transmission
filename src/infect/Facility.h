
class Facility : public Object
{
private:

	int number;
	IntMap *unit;

public:
	Facility(int id)
	{
		number = id;
		unit = new IntMap();
	}

	~Facility()
	{
		for (unit->init(); unit->hasNext(); )
			delete unit->nextValue();
		delete unit;
	}

	void write(ostream &os)
	{
        	os << number;
	}

	inline void addUnit(Unit *u) 
	{ 
		unit->put(u->getId(),u); 
	} 

	inline Unit *getUnit(int i) 
	{ 
		return (Unit *) unit->get(i); 
	} 

	inline IntMap *getUnits() 
	{ 
		unit->init(); 
		return unit; 
	} 

	inline int getId() 
	{	
		return number; 
	}
};
