// infect/SystemEpisodeHistory.h
#ifndef ALUN_INFECT_SYSTEMEPISODEHISTORY_H
#define ALUN_INFECT_SYSTEMEPISODEHISTORY_H

#include "EpisodeHistory.h"

namespace infect
{
class SystemEpisodeHistory : public EpisodeHistory
{
protected:

	virtual void applyInitialEvent(Event *e);
	virtual void unapplyInitialEvent(Event *e);
	virtual void applyAndInsert(HistoryLink *l);
	virtual void removeAndUnapply(HistoryLink *l);

public:

	SystemEpisodeHistory(HistoryLink *aa, HistoryLink *dd): EpisodeHistory(aa,dd)
	{
	}

};

} // namespace infect
#endif // ALUN_INFECT_SYSTEMEPISODEHISTORY_H
