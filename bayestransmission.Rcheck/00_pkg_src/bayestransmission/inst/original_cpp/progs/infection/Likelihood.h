
class Likelihood : public Object
{
private:
	int n;
	Model **m;
	EpisodeEvent **p;
	
	double logLikeAdmit(double imp, double rate, EpisodeEvent *e) 
	{
		double pimp = imp;
	
		if (rate > 0 && e->epis->prev != 0)
		{
			double dt = e->time - e->epis->prev->d->time;
		
			if (e->epis->prev->d->status == 0)
			{
				pimp -= imp * exp(-dt * rate/(1-imp)) ;
			}
			else
			{
				pimp += (1-imp) * exp(-dt * rate/(1-imp)) ;
			}
		}

		return  e->status == 1 ? log(pimp) : log(1-pimp);
	}

public:

	Likelihood(History *h, Model *mod)
	{
		n = 1;
		m = new Model*[n];
		p = new EpisodeEvent*[n+1];
		m[0] = mod;
		p[0] = h->firstEvent();
		p[1] = h->lastEvent();
	}

	Likelihood(Model *mod, EpisodeEvent *f, EpisodeEvent *l)
	{
		n = 1;
		m = new Model*[n];
		p = new EpisodeEvent*[n+1];

		m[0] = mod;
		p[0] = f;
		p[1] = l;
	}

	Likelihood(Model **mods, int nm, EpisodeEvent **pts)
	{
		n = nm;
		m = new Model*[n];
		p = new EpisodeEvent*[n+1];
	
		for (int i=0; i<n; i++)
			m[i] = mods[i];

		for (int i=0; i <= n; i++)
			p[i] = pts[i];
	}


	double logLike(Model *m, EpisodeEvent *e)
	{
		double x = - m->cumCol(e->prev->time,e->time) * e->prev->inf * (e->prev->tot - e->prev->inf);

		x += - m->decol * e->prev->inf * (e->time - e->prev->time);

		switch(e->type)
		{
		case insitu:
			x += e->status == 1 ? m->logpin : m->logoneminpin;
			break;

		case admission:
			x += logLikeAdmit(m->import,m->outrate,e);
			break;

		case colonization:
			x += log(m->instCol(e->time) * e->prev->inf);
			break;

		case decolonization:
			x += m->logdecol;
			break;

		case negtest:
			x += e->status == 1 ? m->logfneg : m->logtneg;
			break;

		case postest:
			x += e->status == 0 ? m->logfpos : m->logtpos;
			break;

		default:
			break;
		}

		return x;
	}

	double logLike(EpisodeEvent *x, EpisodeEvent *y)
	{
		int i = 0;
		while (x->time > p[i+1]->time)
			i++;

		double res = 0;
		for (EpisodeEvent *e = x; e != y; )
		{
			e = e->next;
			if (e->time > p[i+1]->time)
				i++;
			res += logLike(m[i],e);
		}
		return res;
	}

	double logLike(int i)
	{
		return logLike(p[i],p[i+1]);
	}

	double logLike()
	{
		return logLike(p[0],p[n]);
	}

	double logLike(Episode *ep, EpisodeEvent *x, EpisodeEvent *y)
	{
		int i = 0;
		while (x->time > p[i+1]->time)
			i++;

		double res = 0;
		for (EpisodeEvent *e = x; e != y; )
		{
			e = e->next;
			if (e->time > p[i+1]->time)
				i++;
			res += logLike(m[i],e);
		}

		if (y == ep->d && ep->next != 0)
		{
			EpisodeEvent *e = ep->next->a;
			int i = 0;
			while (e->time > p[i+1]->time)
				i++;
			res += logLike(m[i],e);
		}

		return res;
	}

	double logLikeCol(int i)
	{
		double res = 0;
		for (EpisodeEvent *e = p[i]; e != p[i+1]; )
		{
			e = e->next;
			res += - m[i]->cumCol(e->prev->time,e->time) * e->prev->inf * (e->prev->tot - e->prev->inf);
			if (e->type == colonization)
				res += log(m[i]->instCol(e->time) * e->prev->inf);
		}	
		return res;
	}

	double logLikeCol()
	{
		double res = 0;
		for (int i=0; i<n; i++)
			res += logLikeCol(i);
		return res;
	}

	double logLikeAdmit(int i)
	{
		double res = 0;
		for (EpisodeEvent *e = p[i]; e != p[i+1]; )
		{
			e = e->next;
			if (e->type == admission)
				res += logLikeAdmit(m[i]->import,m[i]->outrate,e);
		}
		return res;
	}

	double logLikeAdmit()
	{
		double res = 0;
		for (int i=0; i<n; i++)
			res += logLikeAdmit(i);
		return res;
	}

	string className()
	{
		return "Likelihood";
	}

        void write(ostream &os)
        {
                Object::write(os);
        }
};

