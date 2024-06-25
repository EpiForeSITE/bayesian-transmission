
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
