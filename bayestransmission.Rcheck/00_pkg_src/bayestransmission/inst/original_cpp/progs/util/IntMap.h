
class IntMapLink : public Object
{
friend class IntMap;

private:
	int key;
	Object *value;

	IntMapLink *next;
	IntMapLink *prev;
	IntMapLink *tnext;
	IntMapLink *tprev;

	IntMapLink(int k)
	{
		key = k;
		value = 0;
		prev = 0;
		next = 0;
		tnext = 0;
		tprev = 0;
	}

	string className()
	{
		return "IntMapLink";
	}

	void write(ostream &os)
	{
		Object::write(os);
		os << "(" << key << "->" << value << ")";
	}
};

class IntMap : public Object
{
private:
	static int defcap;

	int cap;
	int use;
	double load;

	IntMapLink **tab;
	IntMapLink *head;
	IntMapLink *tail;
	IntMapLink *current;

	void ensure()
	{
		if (use+1 < cap * load)
			return;

		delete tab;
		cap *= 2;
		tab = new IntMapLink*[cap];
		for (int i=0; i<cap; i++)
			tab[i] = 0;

		for (IntMapLink *l = head; l != 0; l = l->next)
		{
			l->tnext = 0;
			l->tprev = 0;
		}

		for (IntMapLink *l = head; l != 0; l = l->next)
			tabadd(l);
	}

	void tabadd(IntMapLink *l)
	{
		int i = where(l->key);
		if (tab[i] != 0)
			tab[i]->tprev = l;
		l->tnext = tab[i];
		tab[i] = l;
	}

	void tabrem(IntMapLink *l)
	{
		int i = where(l->key);
		if (l->tprev == 0)
			tab[i] = l->tnext;
		else
			l->tprev->tnext = l->tnext;

		if (l->tnext != 0)
			l->tnext->tprev = l->tprev;
	}

	IntMapLink *tabgot(int o)
	{
		for (IntMapLink *l = tab[where(o)]; l != 0; l = l->tnext)
			if (l->key == o)
				return l;
		return 0;
	}

	void listadd(IntMapLink *l)
	{
		l->prev = tail;
		if (tail != 0)
			tail->next = l;
		tail = l;

		if (head == 0)
			head = l;
	}

	void listrem(IntMapLink *l)
	{
		if (l->prev == 0)
			head = l->next;
		else
			l->prev->next = l->next;

		if (l->next == 0)
			tail = l->prev;
		else
			l->next->prev = l->prev;
	}

	int where(int k)
	{
		long res = k % cap;
		while (res < 0)
			res += cap;

		return res;
	}
	
public:
	IntMap(int c = defcap, double l = 0.75) : Object()
	{
		if (c < 1)
			c = 1;

		head = 0;
		tail = 0;

		use = 0;
		load = l;

		cap = c;
		tab = new IntMapLink*[cap];
		for (int i=0; i<cap; i++)
			tab[i] = 0;
	}

	~IntMap()
	{
		delete tab;

		for (IntMapLink *l = head; l != 0; )
		{
			IntMapLink *ll = l;
			l = l->next;
			delete ll;
		}
	}

	void put(int k, Object *v)
	{
		IntMapLink *l = tabgot(k);

		if (l == 0)
		{
			ensure();
			l = new IntMapLink(k);
			use++;
			tabadd(l);
			listadd(l);
		}

		l->value = v;
	}

	void add(int k)
	{
		put(k,0);
	}

	bool got(int k)
	{
		return tabgot(k) != 0;
	}

	Object *get(int k)
	{
		IntMapLink *l = tabgot(k);
		return l == 0 ? 0 : l->value;
	}
	
	Object *remove(int k)
	{
		IntMapLink *l = tabgot(k);
		if (l == 0)
			return 0;

		Object *res = l->value;

		listrem(l);
		tabrem(l);
		delete l;
		use--;

		return res;
	}

	void init()
	{
		current = head;
	}

	bool hasNext()
	{
		return current != 0;
	}

	int next()
	{
		if (current == 0)
			return 0;
		int res = current->key;
		current = current->next;
		return res;
	}

	Object *nextValue()
	{
		if (current == 0)
			return 0;
		Object *res = current->value;
		current = current->next;
		return res;
	}

	string className()
	{
		return "IntMap";
	}

	void write (ostream &os)
	{
		Object::write(os);
		os << "(" << use << "/" << cap << ")";
		for (IntMapLink *l = head; l != 0; l = l->next)
			os << "\n\t" << l;
	}
};

int IntMap::defcap = 10;
