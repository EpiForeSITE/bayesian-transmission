
class Integer : public Object
{
public:
	int value;

	Integer(int x) : Object()
	{
		value = x;
	}

	int intValue()
	{
		return value;
	}

	void set(int n) 
	{
		value = n;
	}

	string className()
	{
		return "Integer";
	}

	void write(ostream &os)
	{
		os << value;
	}
};
