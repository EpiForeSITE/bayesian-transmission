
class Array : public Object
{
protected:
	Object **x;
	int n;

	void error()
	{
		throw(new runtime_error("Array index out of bounds."));
	}

public:

	Array(int c) : Object()
	{
		n = c;
		x = new Object*[n];
		for (int i=0; i<n; i++)
			x[i] = 0;
	}

	~Array()
	{
		if (x)
			delete x;
	}

	void put(int i, Object *o)
	{
		if (i < 0 || i >= n)
			error();
		x[i] = o;
	}

	virtual Object *get(int i)
	{
		if (i < 0 || i >= n)
			error();
		return x[i];
	}

	Object *remove(int i)
	{
		if (i < 0 || i >= n)
			error();

		Object *res = x[i];
		x[i] = 0;
		return res;
	}

	int size()
	{
		return n;
	}

	string className()
	{
		return "Array";
	}

	void write(ostream &os)
	{
		Object::write(os);
		os << "(" << n << ")";
		for (int i=0; i<n; i++)
			os << "\n\t" << x[i];
	}
};
