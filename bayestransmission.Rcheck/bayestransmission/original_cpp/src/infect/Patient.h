
class Patient : public Object
{
private:

	int name;
	int group;
	const static int thou = 1000;

public:
	Patient(int id)
	{
		name = id;
		group = 0;
	}

	~Patient()
	{
	}

	inline int getId() 
	{ 
		return name; 
	} 

	inline void setGroup(int i)
	{
		group = i-thou;
	}

	inline int getGroup()
	{
		return group;
	}

	void write(ostream &os)
	{
        	os << name << "\t" << group;
	}
};
