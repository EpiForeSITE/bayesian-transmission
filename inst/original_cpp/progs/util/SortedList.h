
class SortedList : public List
{
private:
	int carefulCompare(Object *k, Object *p)
	{
		if (k == 0 && p == 0)
			return 0;

		if (p == 0)
			return 1;

		if (k == 0)
			return -1;

		return k->compare(p);
	}

public:
	void append(Object *k)
	{
		ListLink *prev = tail;
		for ( ; prev != 0 && carefulCompare(k,prev->key) < 0; )
			prev = prev->prev;

		ListLink *next = ( prev == 0 ? head : prev->next );
		ListLink *l = new ListLink(k);

		if (prev != 0)
		{
			prev->next = l; 
			l->prev = prev;
		}
		else
		{
			head = l;
		}

		if (next != 0)
		{
			next->prev = l;
			l->next = next;
		}
		else
		{
			tail = l;
		}
	}

	string className()
	{
		return "SortedList";
	}
};
