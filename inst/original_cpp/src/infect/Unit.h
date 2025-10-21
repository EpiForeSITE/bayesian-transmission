
class Unit : public Object
{
private:

	int number;
	Object *f;

public:
	Unit(Object *fac, int id)
	{
		number = id;
		f = fac;
	}

	inline int getId() 
	{	
		return number; 
	}

	inline string getName()
	{
		stringstream ss;
	//	ss << "Unit" <<  number;
		ss << f << ":" << number;
		return ss.str();
	}

	void write(ostream &os)
	{
        	os << f << ":" << number;
	}
};
