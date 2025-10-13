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
    std::cout << "Sampler::initializeEpisodes()" << std::flush;
    std::map<int, Patient*> pos = hist->positives();
    std::cout << "fetching episodes" << std::endl << std::flush;
    for (auto& [ep, eh] : hist->getEpisodes())
    {
        std::cout << "fetching patient "<< std::flush;
        std::cout << eh << std::endl << std::flush;
        std::cout << eh->admissionLink() << std::endl << std::flush;
        std::cout << eh->admissionLink()->getEvent() << std::endl << std::flush;
        std::cout << eh->admissionLink()->getEvent()->getPatient() << std::endl << std::flush;
        Patient* ppp = eh->admissionLink()->getEvent()->getPatient();

        std::cout << ppp << std::endl << std::flush;
        std::cout << "initializingEpisode history for patient " << ppp->getId() << std::endl << std::flush;
        model->initEpisodeHistory(eh, pos.find(ppp->getId()) != pos.end());
    }
    if (model->isCheating())
    {
        std::cout << "Model is cheating." << std::endl;
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
