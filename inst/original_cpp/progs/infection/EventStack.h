

class EventStack
{
protected:
	EpisodeEvent *top;

public:
	EventStack()
	{
		top = 0;
	}

	~EventStack()
	{
		for (EpisodeEvent *x = top; x != 0; )
		{
			EpisodeEvent *y = x;
			x = x->next;
			delete y;
		}	
	}

        virtual EpisodeEvent *pop(EpisodeEvent *x)
	{
		return pop(x->time, x->type, x->epis);
	}

        virtual EpisodeEvent *pop(double t, EventCode tp, Episode *ep)
        {
                EpisodeEvent *e = 0;

                if (top == 0)
                {
                        e = new EpisodeEvent(t,tp,ep);
                        return e;
                }
                else
                {
                        e = top;
                        top = top->next;
                        e->init(t,tp,ep);
                        return e;
                }
        }

        void push(EpisodeEvent *e)
        {
                for (EpisodeEvent *x = e; x != 0; )
                {
                        EpisodeEvent *y = x;
                        x = x->next;
                        y->next = top;
                        top = y;
                }
	}
};
