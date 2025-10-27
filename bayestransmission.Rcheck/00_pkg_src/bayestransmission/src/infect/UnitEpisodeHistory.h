// infect/UnitEpisodeHistory.h
#ifndef ALUN_INFECT_UNITEPISODEHISTORY_H
#define ALUN_INFECT_UNITEPISODEHISTORY_H

#include "EpisodeHistory.h"

class UnitEpisodeHistory : public EpisodeHistory
{
protected:

	virtual void applyInitialEvent(Event *e);
	virtual void unapplyInitialEvent(Event *e);
	virtual void applyAndInsert(HistoryLink *l);
	virtual void removeAndUnapply(HistoryLink *l);

public:

	UnitEpisodeHistory(HistoryLink *aa, HistoryLink *dd): EpisodeHistory(aa,dd)
	{
	}
};

#endif // ALUN_INFECT_UNITEPISODEHISTORY_H
