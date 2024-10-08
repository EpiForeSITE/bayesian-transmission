
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

	string className()
	{
		return "Integer";
	}

	void write(ostream &os)
	{
		os << value;
	}
};
