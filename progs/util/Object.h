
class Object
{
private:
	static long indexcounter;
	long index;

public:
	Object()
	{
		index = ++indexcounter;
	}

	virtual long hash()
	{
		return index;
	}

	virtual int compare(Object *y)
	{
		if (y == 0 || y->index < index)
			return 1;
		if (index < y->index)
			return -1;
		return 0;
	}

	virtual string className()
	{
		return "Object";
	}

	virtual void write(ostream &os)
	{
		os << className() << "[" << index << "]";
	}
	
	friend ostream &operator <<(ostream &os, Object *x);
};

ostream &operator <<(ostream &os, Object *x)
{
	if (x == 0)
	{
		os << "null";
	}
	else
	{
		x->write(os);
	}

	return os;
}

long Object::indexcounter = 0;
