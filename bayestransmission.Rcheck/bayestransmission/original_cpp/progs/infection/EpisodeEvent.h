
class Episode;

class EpisodeEvent : public Event, public EventCoding
{
friend class EventStack;
friend class Episode;
friend class History;
friend class Counter;
friend class EpisodeSampler;
friend class ParameterSampler;
friend class GibbsParameterSampler;
friend class MetroParameterSampler;
friend class Likelihood;

private:
	EventCode type;
	Episode *epis;
	int status;

	EpisodeEvent *prev;
	EpisodeEvent *next;

	double inf;
	double tot;

public:
	EpisodeEvent(double t, EventCode tp, Episode *ep) : Event(t)
	{
		init(t,tp,ep);
	}

	EpisodeEvent(double t, EventCode tp, Episode *ep, EpisodeEvent *nxt) : Event(t)
	{
		init(t,tp,ep);
		next = nxt;
	}

	void init(double t, EventCode tp, Episode *ep)
	{
		time = t;
		type = tp;
		epis = ep;
		status = 0;

		prev = 0;
		next = 0;
		
		inf = 0;
		tot = 0;
	}

	~EpisodeEvent()
	{
		prev = 0;
		next = 0;
	}

	string className()
	{
		return "EpisodeEvent";
	}

	EventCode getType()
	{
		return type;
	}

	void setType(EventCode c)
	{
		type = c;
	}

	double getTime()
	{
		return time;
	}

	EpisodeEvent *getNext()
	{
		return next;
	}

	EpisodeEvent *getPrev()
	{
		return prev;
	}

	Episode *getEpisode()
	{
		return epis;
	}

	int getTotal()
	{
		return tot;
	}

	int getInfected()
	{
		return inf;
	}

	int getStatus()
	{
		return status;
	}

	void setStatus(int s)
	{
		status = s;
	}

	void write(ostream &os);
};
