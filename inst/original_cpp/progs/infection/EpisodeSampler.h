
class EpisodeSampler : public Object
{
private:
	History *h;
	Likelihood *l;
	Random *r;

	void proposeHistory(Episode *ep, int *imp, EpisodeEvent **head, EpisodeEvent **tail, double n, double g)
	{
		*head = *tail = 0;
		*imp = 0;
		int status = 0;

		if (r->runif() < n)
		{
			*imp = 1;
			status = 1;
		}

		int nev = r->rpoisson(g * (ep->d->time - ep->a->time));
		
                double *sorttimes = new double[nev+1];
                sorttimes[0] = r->rexp();
                for (int i=0; i<nev; i++)
                        sorttimes[i+1] = sorttimes[i] + r->rexp();

                for (int i=0; i<nev; i++)
                {
                        double newtime = ep->a->time + sorttimes[i] / sorttimes[nev] * (ep->d->time - ep->a->time);

                        EpisodeEvent *e = h->pop(newtime, (status == 0 ? colonization : decolonization) ,ep);

                        status = 1-status;

                        if ((*head) == 0)
                        {
                                (*head) = (*tail) = e;
                        }
                        else
                        {
                                (*tail)->next = e;
                                e->prev = (*tail);
                                (*tail) = e;
                        }
                }

		delete sorttimes;
	}

	double logProbHistory(Episode *ep, int imp, EpisodeEvent *head, EpisodeEvent *tail, double n, double g)
	{
                int nev = 0;
		for (EpisodeEvent *x = head; x != 0; x = x->next)
			nev++;
		return imp * log(n/(1-n)) + nev * log(g);
	}

	double sampleEpisode(Episode *ep, int max, double imp, double col, double decol)
	{
		EpisodeEvent *head = 0;
		EpisodeEvent *tail = 0;
		int import = 0;
		EpisodeEvent *elo = 0;
		EpisodeEvent *ehi = 0;

		double nu = imp;
		double gamma = (ep->a->inf + ep->d->inf)/2.0 * col + decol;

		proposeHistory(ep,&import,&head,&tail,nu,gamma);

		elo = h->lowerBound(ep,import,head,tail);
		ehi = h->upperBound(ep,import,head,tail);

		double res = 0;
		res = -l->logLike(ep,elo,ehi);

		double accept = -l->logLike(ep,elo,ehi);
		if (!max)
			accept -= logProbHistory(ep,import,head,tail,nu,gamma);

		h->switchHistory(ep,&import,&head,&tail);

		accept += l->logLike(ep,elo,ehi);
		res += l->logLike(ep,elo,ehi);

		if (!max)
			accept += logProbHistory(ep,import,head,tail,nu,gamma);


		if ( ( max ? 0 : log(r->runif()) ) <= accept) 
		{
		//	res = 1;
		}
		else
		{
			h->switchHistory(ep,&import,&head,&tail);
			res = 0;
		}

		h->push(head);
		return res;
	}

public:
	EpisodeSampler(History *hist, Model **mods, int nmods, EpisodeEvent **pts, Random *rand) : Object()
	{
		h = hist;
		l = new Likelihood(mods,nmods,pts);
		r = rand;
	}
	
	EpisodeSampler(History *hist, Model *mod, Random *rand) : Object()
	{
		h = hist;
		l = new Likelihood(mod,h->firstEvent(),h->lastEvent());
		r = rand;
	}

	Likelihood *getLikelihood()
	{
		return l;
	}

	double sampleEpisodes(int max, double imp, double col, double decol) 
	{
		double deltaloglike = 0;

		for (EpisodeEvent *e = h->firstEvent(); e != h->lastEvent(); e = e->next)
			if (e->type == admission || e->type == insitu)
				deltaloglike += sampleEpisode(e->epis,max,imp,col,decol);

		return deltaloglike;
	}
};
