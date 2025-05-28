#include "infect/Unit.h"
#include "infect/HistoryLink.h"

namespace infect{
vector<HistoryLink*> Unit::getHistory() {
    vector<HistoryLink*> history;


    HistoryLink* link = start;
    if (link == nullptr) return history;
    while (link != nullptr) {
        history.push_back(link);
        link = link->uNext();
    }
    return history;
}
} // namespace infect
