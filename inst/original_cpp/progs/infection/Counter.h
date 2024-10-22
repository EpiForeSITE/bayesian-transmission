
class Counter: public Object
{
private:
	int n;
	EpisodeEvent **q;

public:
	Counter(EpisodeEvent *first, EpisodeEvent *last)
	{
		n = 1;
		q = new EpisodeEvent*[n+1];
		q[0] = first;
		q[1] = last;
	}

	Counter(int nreg, EpisodeEvent **pts)
	{
		n = nreg;
		q = pts;
	}

	void countImportationStats(int i, double *a, double *b)
	{
		for (EpisodeEvent *e = q[i]; e != q[i+1]; e = e->next)
		{
			if (e->type == admission)
			{
				if (e->status == 0)
					(*b)++;
				else
					(*a)++;
			}
		}
	}

	void countImportationStats(double *a, double *b)
	{
		for (int i=0; i<n; i++)
			countImportationStats(i,a,b);
	}

	void countInsituStats(int i, double *a, double *b)
	{
		for (EpisodeEvent *e = q[i]; e != q[i+1]; e = e->next)
		{
			if (e->type == insitu)
			{
				if (e->status == 0)
					(*b)++;
				else
					(*a)++;
			}
		}
	}

	void countInsituStats(double *a, double *b)
	{
		for (int i=0; i<n; i++)
			countInsituStats(i,a,b);
	}

	void countFalseNegStats(int i, double *a, double *b)
	{
		for (EpisodeEvent *e = q[i]; e != q[i+1]; e = e->next)
		{
			if (e->status == 1)
			{
				switch(e->type)
				{
				case postest:
					(*b)++;
					break;
				case negtest:
					(*a)++;
					break;
				default:
					break;
				}
			}
		}
	}

	void countFalseNegStats(double *a, double *b)
	{
		for (int i=0; i<n; i++)
			countFalseNegStats(i,a,b);
	}

	void countFalsePosStats(int i, double *a, double *b)
	{
		for (EpisodeEvent *e = q[i]; e != q[i+1]; e = e->next)
		{
			if (e->status == 0)
			{
				switch(e->type)
				{
				case postest:
					(*a)++;
					break;
				case negtest:
					(*b)++;
					break;
				default:
					break;
				}
			}
		}
	}

	void countFalsePosStats(double *a, double *b)
	{
		for (int i=0; i<n; i++)
			countFalsePosStats(i,a,b);
	}

	void countColonizationStats(int i, double *shape, double *rate)
	{
		for (EpisodeEvent *p = q[i]; p != q[i+1]; p = p->next)
		{
			EpisodeEvent *e = p->next;
			(*rate) += (e->time - p->time) * p->inf * (p->tot - p->inf);
			(*shape) += ( p->type == colonization ? 1 : 0);
		}
	}

	void countColonizationStats(double *shape, double *rate)
	{
		for (int i=0; i<n; i++)
			countColonizationStats(i,shape,rate);
	}

	void countDecolonizationStats(int i, double *shape, double *rate)
	{
		for (EpisodeEvent *p = q[i]; p != q[i+1]; p = p->next)
		{
			EpisodeEvent *e = p->next;
			(*rate) += (e->time - p->time) * p->inf;
			(*shape) += ( p->type == decolonization ? 1 : 0);
		}
	}

	void countDecolonizationStats(double *shape, double *rate)
	{
		for (int i=0; i<n; i++)
			countDecolonizationStats(i,shape,rate);
	}

	void countSummaryStats(int i, double *patdays, double *patdaysinf, double *patdayssus, int *cols, int *decols)
	{
		for (EpisodeEvent *p = q[i]; p != q[i+1]; p = p->next)
		{
			EpisodeEvent *e = p->next;
			(*patdays) += (e->time - p->time) * p->tot;
			(*patdaysinf) += (e->time - p->time) * p->inf;
			(*patdayssus) += (e->time - p->time) * (p->tot - p->inf);
			(*cols) += (p->type == colonization ? 1 : 0);
			(*decols) += (p->type == decolonization ? 1 : 0);
		}
	}

	void countSummaryStats(double *patdays, double *patdaysinf, double *patdayssus, int *cols, int *decols)
	{
		for (int i=0; i<n; i++)
			countSummaryStats(i,patdays,patdaysinf,patdayssus,cols,decols);
	}

// Class handling stuff.

	string className()
	{
		return "Counter";
	}

	void write(ostream &os)
	{
		Object::write(os);
	}
};
