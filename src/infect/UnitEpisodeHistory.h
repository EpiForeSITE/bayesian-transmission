

class UnitEpisodeHistory : public EpisodeHistory
{
protected:

	virtual void applyInitialEvent(Event *e)
	{
		for (HistoryLink *l = d; ; l = l->uPrev())
		{
			if (l->getPState() != 0)
				l->getPState()->apply(e);

			if (l != d)
			{
				if (l->getUState() != 0)
					l->getUState()->apply(e);
			}

			if (l == a)
			{
				break;
			}
		}
	}

	virtual void unapplyInitialEvent(Event *e)
	{
		for (HistoryLink *l = a;  ; l = l->uNext())
		{
			if (l->getPState() != 0)
				l->getPState()->unapply(e);

			if (l != d)
			{
				if (l->getUState() != 0)
					l->getUState()->unapply(e);
			}
			else
			{
				break;
			}
		}
	}

	virtual void applyAndInsert(HistoryLink *l)
	{
		HistoryLink *unext = d;
		HistoryLink *pnext = d;

		for (HistoryLink *ll = d; ll != a; )
		{
			unext = ll;

			if (ll->getEvent()->getPatient() == l->getEvent()->getPatient())
				pnext = ll;

			if (ll != d)
			{
				if (ll->getUState() != 0)
					ll->getUState()->apply(l->getEvent());
			}

			if (ll->getPState() != 0)
				ll->getPState()->apply(l->getEvent());

			if (l->getEvent()->getTime() < ll->uPrev()->getEvent()->getTime())
			{
				ll = ll->uPrev();
			}
			else
			{
				break;
			}
		}

		l->insertBeforeU(unext);
		if (l->getUState() != 0)
		{
			l->getUState()->copy(l->uPrev()->getUState());
			l->getUState()->apply(l->getEvent());
		}

		l->insertBeforeP(pnext);
		if (l->getPState() != 0)
		{
			l->getPState()->copy(l->pPrev()->getPState());
			l->getPState()->apply(l->getEvent());
		}
	}

	virtual void removeAndUnapply(HistoryLink *l)
	{
		l->removeP();
		l->removeU();

		for (HistoryLink *ll = l->uNext(); ; ll = ll->uNext())
		{
			if (ll->getPState() != 0)
				ll->getPState()->unapply(l->getEvent());

			if (ll != d)
			{
			 	if (ll->getUState() != 0)
					ll->getUState()->unapply(l->getEvent());
			}
			else
			{
				break;
			}
		}
	}

public:

	UnitEpisodeHistory(HistoryLink *aa, HistoryLink *dd): EpisodeHistory(aa,dd)
	{
	}
};
