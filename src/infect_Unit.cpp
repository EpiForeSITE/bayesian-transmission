#include "infect/Unit.h"
#include "infect/HistoryLink.h"

namespace infect{
vector<HistoryLink*> Unit::getHistory(Unit* x) {
    vector<HistoryLink*> history;


    HistoryLink* link = x->getStart();
    if (link == nullptr) return history;
    while (link != nullptr) {
        history.push_back(link);
        link = link->uNext();
    }
    return history;
}
} // namespace infect
