
class ListLink : public Object
{
friend class List;
friend class SortedList;

private:
	Object *key;
	ListLink *next;
	ListLink *prev;

	ListLink(Object *k)
	{
		key = k;
		prev = 0;
		next = 0;
	}

public:
	Object *getKey()
	{
		return key;
	}

	string className()
	{
		return "ListLink";
	}

	void write(ostream &os)
	{
		Object::write(os);
		os << "(" << key << ")";
	}
};

class List : public Object
{
protected:
	ListLink *head;
	ListLink *tail;

private:
	ListLink *current;

	ListLink *listget(Object *o)
	{
		for (ListLink *l = head; l != 0; l = l->next)
			if (l->key == o)
				return l;
		return 0;
	}

public:
	List() : Object()
	{
		head = 0;
		tail = 0;
		current = 0;
	}

	~List()
	{
		for (ListLink *l = head; l != 0; )
		{
			ListLink *ll = l;
			l = l->next;
			delete ll;
		}
	}

	List *copy()
	{
		List *l = new List();
		for (init(); hasNext(); )
			l->append(next());
		return l;
	}

	Object *random(Random *r)
	{
		int x = size();
		if (x == 0)
			return 0;

		double u = r->runif() * x;
		x = 0;

		for (ListLink *l = head; l != 0; l = l->next)
		{
			if (u <= ++x)
				return l->key;
		}

		return 0;
	}

	virtual void append(Object *k)
	{
		ListLink *l = new ListLink(k);
	
		if (head == 0)
			head = l;

		if (tail != 0)
		{
			l->prev = tail;
			tail->next = l;
		}
		tail = l;
	}

	Object *remove(Object *k)
	{
		ListLink *l = listget(k);
		if (l == 0)
			return 0;

		Object *res = l->key;

		if (l->prev == 0)
			head = l->next;
		else
			l->prev->next = l->next;
		
		if (l->next == 0)
			tail = l->prev;
		else
			l->next->prev = l->prev;

		delete l;

		return res;
	}

	bool contains(Object *k)
	{
		ListLink *l = head;
		for ( ; l != 0; l = l->next)
			if (l->key == k)
				return true;
		return false;
	}

	void init()
	{
		current = head;
	}

	bool hasNext()
	{
		return current != 0;
	}

	Object *next()
	{
		if (current == 0)
			return 0;

		Object *res = current->key;
		current = current->next;
		return res;
	}

	bool isEmpty()
	{
		return head == 0;
	}

	int size()
	{
		int count = 0;
		for (ListLink *l = head; l != 0; l = l->next)
			count++;
		return count;
	}

	Object *pop()
	{
		if (head == 0)
			return 0;

		ListLink *l = head;

		head = head->next;

		if (head != 0)
			head->prev = 0;
		else
			tail = 0;
		
		Object *res = l->key;
		delete l;

		return res;
	}
		
	string className()
	{
		return "List";
	}

	void write(ostream &os)
	{
		Object::write(os);
		for (ListLink *l = head; l != 0; l = l->next)
			os << "\n\t" << l;
	}
};
