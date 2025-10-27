
class MapLink : public Object
{
friend class Map;

private:
	Object *key;
	Object *value;

	MapLink *next;
	MapLink *prev;
	MapLink *tnext;
	MapLink *tprev;

	MapLink(Object *k)
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
		return "MapLink";
	}

	void write(ostream &os)
	{
		Object::write(os);
		os << "(" << key << "->" << value << ")";
	}
};

class Map : public Object
{
private:
	static int defcap;

	int cap;
	int use;
	double load;

	MapLink **tab;
	MapLink *head;
	MapLink *tail;
	MapLink *current;

	void ensure()
	{
		if (use+1 < cap * load)
			return;

		delete tab;
		cap *= 2;
		tab = new MapLink*[cap];
		for (int i=0; i<cap; i++)
			tab[i] = 0;

		for (MapLink *l = head; l != 0; l = l->next)
		{
			l->tnext = 0;
			l->tprev = 0;
		}

		for (MapLink *l = head; l != 0; l = l->next)
			tabadd(l);
	}

	void tabadd(MapLink *l)
	{
		int i = where(l->key);
		if (tab[i] != 0)
			tab[i]->tprev = l;
		l->tnext = tab[i];
		tab[i] = l;
	}

	void tabrem(MapLink *l)
	{
		int i = where(l->key);
		if (l->tprev == 0)
			tab[i] = l->tnext;
		else
			l->tprev->tnext = l->tnext;

		if (l->tnext != 0)
			l->tnext->tprev = l->tprev;
	}

	MapLink *tabgot(Object *o)
	{
		for (MapLink *l = tab[where(o)]; l != 0; l = l->tnext)
			if (l->key == o)
				return l;
		return 0;
	}

	void listadd(MapLink *l)
	{
		l->prev = tail;
		if (tail != 0)
			tail->next = l;
		tail = l;

		if (head == 0)
			head = l;
	}

	void listrem(MapLink *l)
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

	int where(Object *o)
	{
		if (o == 0)
			return 0;

		long res = o->hash() % cap;
		while (res < 0)
			res += cap;

		return res;
	}
	
public:
	Map(int c = defcap, double l = 0.75) : Object()
	{
		if (c < 1)
			c = 1;

		head = 0;
		tail = 0;

		use = 0;
		load = l;

		cap = c;
		tab = new MapLink*[cap];
		for (int i=0; i<cap; i++)
			tab[i] = 0;
	}

	~Map()
	{
		delete tab;

		for (MapLink *l = head; l != 0; )
		{
			MapLink *ll = l;
			l = l->next;
			delete ll;
		}
	}

	Map *copy()
	{
		Map *x = new Map(size(),0.75);
		for (init(); hasNext(); )
		{
			Object *k = next();
			x->put(k,get(k));
		}

		return x;
	}

	void put(Object *k, Object *v)
	{
		MapLink *l = tabgot(k);

		if (l == 0)
		{
			ensure();
			l = new MapLink(k);
			use++;
			tabadd(l);
			listadd(l);
		}

		l->value = v;
	}

	Object *get(Object *k)
	{
		MapLink *l = tabgot(k);
		return l == 0 ? 0 : l->value;
	}
	
	void add(Object *k)
	{
		put(k,0);
	}

	bool got(Object *k)
	{
		return tabgot(k) != 0;
	}

	Object *remove(Object *k)
	{
		MapLink *l = tabgot(k);
		if (l == 0)
			return 0;

		Object *res = l->key;

		listrem(l);
		tabrem(l);
		delete l;
		use--;

		return res;
	}

	void change(int opt, Object *k)
	{
		if (opt > 0)
			add(k);
		else if (opt < 0)
			remove(k);
	}

	int size()
	{
		return use;
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
		return "Map";
	}

	void write (ostream &os)
	{
		Object::write(os);
		os << "(" << use << "/" << cap << ")";
		for (MapLink *l = head; l != 0; l = l->next)
			os << "\n\t" << l;
	}
};

int Map::defcap = 10;
