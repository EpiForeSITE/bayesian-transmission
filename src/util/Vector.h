
class Vector : public Object
{
private:
	static int defcap;

	Object **x;
	int cap;
	int n;

	void ensure(int m)
	{
		if (m <= cap)
			return;
		
		int newcap = cap;
		while (m > newcap)
			newcap *= 2;

		Object **newx = new Object*[newcap];

		for (int i=0; i<n; i++)
			newx[i] = x[i];
		for (int i=n; i<newcap; i++)
			newx[i] = 0;

		delete x;
		x = newx;
		cap = newcap;
	}

public:
	Vector(int c = defcap) : Object()
	{
		cap = c;
		n = 0;
		x = new Object*[cap];
		for (int i=0; i<cap; i++)
			x[i] = 0;
	}

	~Vector()
	{
		if (x)
			delete x;
	}

	inline void add(Object *o)
	{
		ensure(n+1);
		x[n++] = o;
	}

	inline Object *remove(Object *o)
	{
		int i=0;
		Object *res = 0;

		for (; i<n; i++)
			if (x[i] == o)
			{
				res = o;
				n--;
				for (; i<n; i++)
					x[i] = x[i+1];
				x[i] = 0;
				break;
			}

		return  res;
	}

	inline int size()
	{
		return n;
	}

	inline Object *get(int i)
	{
		return x[i];
	}

	inline void put(int i, Object *o)
	{
		ensure(i+1);
		x[i] = o;
	}

	string className()
	{
		return "Vector";
	}

	void write(ostream &os)
	{
		Object::write(os);
		os << "(" << n << "/" << cap << ")";
		for (int i=0; i<size(); i++)
			os << "\n\t" << x[i];
	}
};
