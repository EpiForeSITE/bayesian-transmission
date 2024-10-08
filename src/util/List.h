
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
	inline Object *getKey()
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

	inline ListLink *listget(Object *o)
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
		clear();
	}

	inline void clear()
	{
		for (ListLink *l = head; l != 0; )
		{
			ListLink *ll = l;
			l = l->next;
			delete ll;
		}
		head = 0;
		tail = 0;
		current = 0;
	}

	inline List *copy()
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

	inline virtual void append(Object *k)
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

	inline virtual void prepend(Object *k)
	{
		ListLink *l = new ListLink(k);
		
		if (tail == 0)
			tail = l;

		if (head != 0)
		{
			l->next = head;
			head->prev = l;
		}
		head = l;
	}

	inline Object *remove(Object *k)
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

	inline bool contains(Object *k)
	{
		ListLink *l = head;
		for ( ; l != 0; l = l->next)
			if (l->key == k)
				return true;
		return false;
	}

	inline void init()
	{
		current = head;
	}

	inline void initAtTail()
	{
		current = tail;
	}


	inline Object *getFirst()
	{
		return head == 0 ? 0 : head->key ;
	}

	inline Object *getLast()
	{
		return tail == 0 ? 0 : tail->key ;
	}

	inline bool hasNext()
	{
		return current != 0;
	}

	inline bool hasPrev()
	{
		return current != 0;
	}

	inline Object *next()
	{
		if (current == 0)
			return 0;

		Object *res = current->key;
		current = current->next;
		return res;
	}

	inline Object *prev()
	{
		if (current == 0)
			return 0;

		Object *res = current->key;
		current = current->prev;
		return res;
	}

	inline bool isEmpty()
	{
		return head == 0;
	}

	inline int size()
	{
		int count = 0;
		for (ListLink *l = head; l != 0; l = l->next)
			count++;
		return count;
	}

	inline Object *pop()
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
