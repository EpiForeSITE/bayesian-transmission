
class Integer : public Object
{
public:
	int value;

	Integer(int x) : Object()
	{
		value = x;
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
