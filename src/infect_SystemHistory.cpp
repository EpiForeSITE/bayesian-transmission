#include "infect/infect.h"

namespace infect {

HistoryLink* SystemHistory::makeHistoryLink(Model *mod, Event *e)
{
    if (mod == 0)
    {
        return new HistoryLink(e,0,0,0,0);
    }
    else
    {
        return new HistoryLink
        (
                e,
                mod->makeSystemState(),
                mod->makeFacilityState(e->getFacility()),
                mod->makeUnitState(e->getUnit()),
                mod->makePatientState(e->getPatient())
        );
    }
}

HistoryLink* SystemHistory::makeHistoryLink(Model *mod, Facility *f, Unit *u, double t, Patient *p, EventCode c)
{
    Event *e = new Event(f,u,t,p,c);
    HistoryLink *h = makeHistoryLink(mod,e);
    mylinks.push_back(h);
    return h;
}

int SystemHistory::needEventType(EventCode e)
{
    switch(e)
    {
    case admission:
    case admission0:
    case admission1:
    case admission2:
    case insitu:
    case insitu0:
    case insitu1:
    case insitu2:
    case discharge:
    case start:
    case stop:
    case marker:
        return 1;

    default:
        return 0;
    }
}


SystemHistory::~SystemHistory()
{
    // Clean up dynamically allocated memory if necessary
    for (auto& [key, ep] : ep2ephist) {
        delete ep;
    }
    for (auto& [key, link] : pheads) {
        delete link;
    }
    for (auto& [key, link] : uheads) {
        delete link;
    }
    for (auto& [key, link] : fheads) {
        delete link;
    }
    for (auto& link : mylinks) {
        delete link;
    }
}


SystemHistory::SystemHistory(System *s, Model *m, bool verbose)
{
    Map *tails = new Map();


    shead = makeHistoryLink(m,0,0,s->startTime(),0,start);
    HistoryLink *stail = makeHistoryLink(m,0,0,s->endTime(),0,stop);
    shead->insertBeforeS(stail);


    for (auto& [key, facility] : s->getFacilities()) {
        HistoryLink *fhead = makeHistoryLink(m, facility, 0, s->startTime(), 0, start);
        HistoryLink *ftail = makeHistoryLink(m, facility, 0, s->endTime(), 0, stop);
        fhead->insertBeforeF(ftail);
        fheads[facility->getId()] = fhead;
        tails->put(facility, ftail);

        for (auto& [unitKey, unit] : facility->getUnits()) {
            HistoryLink* uhead = makeHistoryLink(m, facility, unit, s->startTime(), 0, start);
            HistoryLink* utail = makeHistoryLink(m, facility, unit, s->endTime(), 0, stop);
            uhead->insertBeforeU(utail);
            uheads[unit->getId()] = uhead;
            tails->put(unit, utail);
            unit->setStart(uhead);
        }
    }


    HistoryLink **hx = new HistoryLink*[s->getPatients().size()];
    int hxn = 0;
    for (auto& [key, patient] : s->getPatients()) {
        HistoryLink *prev = 0;
        for (Map *episodes = (Map *) s->getEpisodes(patient); episodes->hasNext();)
        {
            Episode *ep = (Episode *) episodes->next();
            for (SortedList *t = ep->getEvents(); t->hasNext(); )
            {
                Event *e = (Event *) t->next();
                HistoryLink *x = makeHistoryLink(m,e);
                if (prev == 0)
                {
                    pheads[patient->getId()] = x;
                }
                else
                {
                    x->insertAfterP(prev);
                }

                if (ep2adm.find(ep) == ep2adm.end())
                {
                    ep2adm[ep] = x;
                    adm2ep[x] = ep;
                }
                prev = x;
            }
            ep2dis[ep] = prev;
        }
        hx[hxn++] = pheads[patient->getId()];
    }

    for (int i=0; i<hxn; i++)
    {
        for (int j=i; j>0; j--)
        {
            if (hx[j]->getEvent()->getTime() < hx[j-1]->getEvent()->getTime())
            {
                HistoryLink *y = hx[j-1];
                hx[j-1] = hx[j];
                hx[j] = y;
            }
            else
            {
                break;
            }
        }
    }

    for (int i=0; i<hxn; )
    {
        hx[i]->insertBeforeS(stail);
        hx[i]->insertBeforeF((HistoryLink *)tails->get(hx[i]->getEvent()->getFacility()));
        hx[i]->insertBeforeU((HistoryLink *)tails->get(hx[i]->getEvent()->getUnit()));

        hx[i] = hx[i]->pNext();

        if (hx[i] == 0)
        {
            i++;
        }
        else
        {
            for (int j=i+1; j<hxn; j++)
            {
                if (hx[j]->getEvent()->getTime() < hx[j-1]->getEvent()->getTime())
                {
                    HistoryLink *y = hx[j-1];
                    hx[j-1] = hx[j];
                    hx[j] = y;
                }
                else
                {
                    break;
                }
            }
        }
    }

    // Put the sub lists into the full lists.


    for (auto& [key, facility] : s->getFacilities()) {
        HistoryLink *fhead = fheads[facility->getId()];
        HistoryLink *ftail = (HistoryLink *) tails->get(facility);
        ftail->insertBeforeS(stail);
        fhead->insertBeforeS(shead->sNext());
        for (auto& [unitKey, unit] : facility->getUnits()) {
            HistoryLink *uhead = uheads[unit->getId()];
            HistoryLink *utail = (HistoryLink *) tails->get(unit);
            utail->insertBeforeF(ftail);
            uhead->insertBeforeF(fhead->fNext());
            utail->insertBeforeS(ftail);
            uhead->insertBeforeS(fhead->sNext());
        }
    }

    // Filter events model then propagate events.

    if (m != 0)
    {
        for (HistoryLink *l = getSystemHead(); l != 0; )
        {
            if (needEventType(l->getEvent()->getType()) || m->needEventType(l->getEvent()->getType()))
            {
                l = l->sNext();
            }
            else
            {
                if (verbose)
                    cout << "Removing un needed event \t" << l->getEvent() << "\n";

                HistoryLink *ll = l;
                l = l->sNext();
                ll->remove();
                delete ll;
            }
        }
    }

    if (m != 0)
        m->handleAbxDoses(getSystemHead());

    for (HistoryLink *l = getSystemHead(); l != 0; l = l->sNext())
        l->setCopyApply();

    // Make episode histories.

    if (m != 0)
    {
        for (auto& [ep, adm] : ep2adm)
        {
            EpisodeHistory *eh = m->makeEpisodeHistory(ep2adm[ep], ep2dis[ep]);
            ep2ephist[ep] = eh;
        }
    }

    // Clean up.

    delete [] hx;
    delete tails;
}

EpisodeHistory** SystemHistory::getPatientHistory(Patient *pat, int *n)
{
    int k = 0;
    for (HistoryLink *l = pheads[pat->getId()]; l != 0; l = l->pNext())
        if (l->getEvent()->isAdmission() || l->getEvent()->isInsitu())
            k++;
    *n = k;

    EpisodeHistory **eps = new EpisodeHistory*[k];
    k = 0;
    for (HistoryLink *l = pheads[pat->getId()]; l != 0; l = l->pNext())
        if (l->getEvent()->isAdmission() || l->getEvent()->isInsitu())
            eps[k++] = ep2ephist[adm2ep[l]];

    return eps;
}

List* SystemHistory::getTestLinks()
{
    List *res = new List();

    for (HistoryLink *l = shead ; l != 0; l = l->sNext())
    {
        if (l->getEvent()->isTest())
            res->append(l);
    }

    return res;
}


std::map<int, Patient*> SystemHistory::positives()
{
    std::map<int, Patient*> pos;
    for (HistoryLink *l = getSystemHead(); l != 0; l = l->sNext())
    {
        if (l->getEvent()->isPositiveTest())
            pos[l->getEvent()->getPatient()->getId()] = l->getEvent()->getPatient();
    }
    return pos;
}


int SystemHistory::sumocc()
{
    int x = 0;

    for (HistoryLink *l = shead; l != 0; l = l->sNext())
        if (l->getUState() != 0)
            x += l->getUState()->getTotal();

    return x;
}

void SystemHistory::write(ostream &os)
{
    write2(os,0);
}

void SystemHistory::write2(ostream &os, int opt)
{
    std::map<Object*, int> count;
    switch(opt)
    {
    case 9: // All events. With count data influencing performance.
        for (auto& [key, link] : uheads)
        {
            count[link] = 0;
            cerr << count[link] << "\n";
        }
        for (HistoryLink *l = shead; l != 0; l = l->sNext())
        {
            l->getEvent()->write(os);
            os << "\t";
            int cc = count[l->getEvent()->getUnit()];
            if (cc != 0)
                cc = l->getUState()->getTotal();
            for (auto& [key, value] : count)
                os << "\t" << value;
            os << "\n";
        }
        break;
    case 8: // By unit. All link data.
        for (auto& [key, link] : uheads)
            for (HistoryLink* l = link; l != 0; l = l->pNext())
            {
                l->write2(os, 1);
                os << "\n";
            }
        break;
    case 7: // By unit. All events.
        for (auto& [key, link] : uheads)
            for (HistoryLink *l = link; l != 0; l = l->pNext())
            {
                l->getEvent()->write2(os, 1);
                os << "\n";
            }
        break;
    case 6: // By unit. Observable events only.
        for (auto& [key, link] : uheads)
            for (HistoryLink *l = link; l != 0; l = l->pNext())
                if (l->getEvent()->isObservable())
                    os << l->getEvent() << "\n";
        break;
    case 5: // By patient. All link data.
        for (auto& [key, link] : pheads)
            for (HistoryLink *l = link; l != 0; l = l->pNext())
            {
                l->write2(os, 1);
                os << "\n";
            }
        break;
    case 4: // By patient. All events.
        for (auto& [key, link] : pheads)
            for (HistoryLink *l = link; l != 0; l = l->pNext())
            {
                l->getEvent()->write2(os, 1);
                os << "\n";
            }
        break;
    case 3: // By patient. Observable events only.
        for (auto& [key, link] : pheads)
            for (HistoryLink *l = link; l != 0; l = l->pNext())
                if (l->getEvent()->isObservable())
                    os << l->getEvent() << "\n";
        break;
    case 2: // By system, all events and link states. NB: Not all events may be reachable by system links.
        for (HistoryLink *l = shead; l != 0; l = l->sNext())
        {
            l->write2(os, 1);
            os << "\n";
        }
        break;
    case 1: // By system. All events. NB: Not all events may be reachable by system links.
        for (HistoryLink *l = shead; l != 0; l = l->sNext())
        {
            l->getEvent()->write2(os, 1);
            os << "\n";
        }
        break;
    case 0: // By system, observable events only.
    default:
        for (HistoryLink *l = shead; l != 0; l = l->sNext())
            if (l->getEvent()->isObservable())
                os << l->getEvent() << "\n";
        break;
    }
}

vector<Unit*> SystemHistory::getUnits(){
    vector<Unit*> units;
    for (auto& [key, link] : uheads)
        units.push_back(link->getEvent()->getUnit());
    return units;
}

} // namespace infect
