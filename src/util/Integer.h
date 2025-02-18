
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

	string className() const
	{
		return "Integer";
	}

	void write(ostream &os) const
	{
		os << value;
	}
};
