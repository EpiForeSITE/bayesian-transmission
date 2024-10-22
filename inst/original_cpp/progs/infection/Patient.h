
class Episode;

class Patient : public Object
{
friend class RawEventList;

private:
	int index;

	Episode *h;
	Episode *t;

public:
	Patient(int id)
	{
		index = id;
		h = 0;
		t = 0;
	}

	int id()
	{	
		return index;
	}

	string className()
	{
		return "Patient";
	}

	Episode *firstEpisode()
	{
		return h;
	}

	void write(ostream &os);
};
