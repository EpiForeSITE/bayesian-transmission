#include "infect/infect.h"
#include <exception>
namespace infect {

Sampler::Sampler(SystemHistory *h, Model *m, Random *r)
    : hist(h), model(m), rand(r)
{
    initializeEpisodes();
}


void Sampler::sampleModel()
{
    sampleModel(0);
}

void Sampler::sampleModel(int max)
{
    model->update(hist,rand,max);
}

void Sampler::sampleEpisodes()
{
    sampleEpisodes(0);
}


void Sampler::sampleEpisodes(int max)
{
    model->sampleEpisodes(hist,max,rand);
}

void Sampler::initializeEpisodes()
{
    std::map<int, Patient*> pos = hist->positives();
    for (auto& [ep, eh] : hist->getEpisodes())
    {
        Patient *ppp = eh->admissionLink()->getEvent()->getPatient();
        model->initEpisodeHistory(eh, pos[ppp->getId()]);
    }
    if (model->isCheating())
    {
        for (auto& [ep, eh] :  hist->getEpisodes())
        {
            eh->unapply();
        }
        for (HistoryLink *l = hist->getSystemHead(); l != 0; l = l->sNext())
            l->setCopyApply();
        for (auto& [ep, eh] :  hist->getEpisodes())
        {
            eh->apply();
        }
    }
}

} // namespace infect
