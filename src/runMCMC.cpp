
#include <string>
using std::string;

#include "util/util.h"
#include "infect/infect.h"
#include "modeling/modeling.h"
#include "lognormal/lognormal.h"

using namespace infect;
using namespace util;
using namespace lognormal;

#include "wrap.h"

#include <Rcpp.h>
using namespace Rcpp;

#include "RRandom.h"



lognormal::LogNormalModel* newModel(
        std::string modname,
        int nstates,
        Rcpp::List modelParameters) //< Model specific options.
{
    lognormal::LogNormalModel *model = 0;

    // if (modname == "LinearAbxModel")
    // {
    //     model = new lognormal::LinearAbxModel(nstates,nmetro,forward,cheat);
    // } else
    // if (modname == "LinearAbxModel2")
    // {
    //     model = new lognormal::LinearAbxModel2(nstates,nmetro,forward,cheat);
    // } else
    // if (modname == "MultiUnitLinearAbxModel")
    // {
    //     model = new lognormal::MultiUnitLinearAbxModel(nstates,l,nmetro,forward,cheat);
    // } else
    if (modname == "MixedModel")
    {
        model = new lognormal::MixedModel(
            nstates,
            modelParameters["nmetro"],
            modelParameters["forward"],
            modelParameters["cheat"]
        );
    } else
    // if (modname == "LogNormalAbxModel")
    // {
    //     model = new lognormal::LogNormalAbxModel(nstates,nmetro,forward,cheat);
    // } else
    // if (modname == "LNMassActionModel")
    // {
    //     model = new lognormal::LNMassActionModel(nstates,nmetro,forward,cheat);
    // } else
    if (modname == "LogNormalModel")
    {
        model = new lognormal::LogNormalModel(
            nstates,
            modelParameters["nmetro"],
            modelParameters["forward"],
            modelParameters["cheat"]
        );
    }
    else
    {
        throw std::invalid_argument("Invalid model name");
    }

    //model->setup(modOptions);

    return model;
}

// [[Rcpp::export]]
DataFrame runMCMC(
    std::string modname,
    Rcpp::DataFrame data,
    Rcpp::List MCMCParameters,
    Rcpp::List modelParameters,
    int nstates = 2,
    bool verbose = false
) {
    if(verbose)
        Rcpp::message(Rcpp::wrap(string("Initializing Variables")));

    // Make random number generator.

    RRandom *random = new RRandom();


    System *sys = new System(
        as<std::vector<int>>(data["facilities"]),
        as<std::vector<int>>(data["units"]),
        as<std::vector<double>>(data["times"]),
        as<std::vector<int>>(data["patients"]),
        as<std::vector<int>>(data["types"])
    );


    //Model
    lognormal::LogNormalModel *model = newModel(modname, nstates, modelParameters);

    // Set time origin of model.
    LogNormalICP *icp = (LogNormalICP *) model->getInColParams();
    icp->setTimeOrigin((sys->endTime()-sys->startTime())/2.0);


    // Create state history.

    if (verbose)
        Rcpp::message(Rcpp::wrap(string("Building history structure.")));

    SystemHistory *hist = new SystemHistory(sys, model, false);

    // Find tests for posterior prediction and, hence, WAIC estimates.

    if (verbose)
        Rcpp::message(Rcpp::wrap(string("Finding tests for WAIC.\n")));

    util::List *tests = hist->getTestLinks();
    TestParams **testtype = new TestParams*[tests->size()];
    HistoryLink **histlink = new HistoryLink*[tests->size()];

    int wntests = 0;
    double wprob = 0;
    double wlogprob = 0;
    double wlogsqprob = 0;
    for (tests->init(); tests->hasNext(); wntests++)
    {
        histlink[wntests] = (HistoryLink *) tests->next();

        if (histlink[wntests]->getEvent()->isClinicalTest())
            testtype[wntests] = model->getClinicalTestParams();
        else
            testtype[wntests] = model->getSurveilenceTestParams();
    }

    // Make and runsampler.

    if (verbose)
        Rcpp::message(Rcpp::wrap(string("Building sampler.\n")));

    Sampler *mc = new Sampler(hist,model,random);

    if (verbose)
        Rcpp::message(Rcpp::wrap(string("burning in MCMC.\n")));
    unsigned int nburn = MCMCParameters["nburn"];
    for (unsigned int i=0; i<nburn; i++)
    {
        mc->sampleEpisodes();
        mc->sampleModel();
    }

    if (verbose)
        Rcpp::message(Rcpp::wrap(string("Running MCMC.\n")));

    bool outputparam = MCMCParameters["outputparam"];
    unsigned int nsims = MCMCParameters["nsims"];

    Rcpp::List paramchain(nsims);
    Rcpp::NumericVector llchain(nsims);

    for (unsigned int i=0; i<nsims; i++)
    {
        mc->sampleEpisodes();
        mc->sampleModel();


        if (outputparam)
        {
        //     cout << model << "\t\t" << model->logLikelihood(hist) << "\n";
        //     cout.flush();
            paramchain(i) = model2R(model);
            llchain(i) = model->logLikelihood(hist);
        }


        for (int j=0; j<wntests; j++)
        {
            HistoryLink *hh = histlink[j];
            double p = testtype[j]->eventProb(hh->getPState()->infectionStatus(),hh->getPState()->onAbx(),hh->getEvent()->getType());
            wprob += p;
            wlogprob += log(p);
            wlogsqprob += log(p)*log(p);
        }
    }

  return Rcpp::DataFrame::create(
    // Named("verbose") = verbose,
    // Named("nmetro") = nmetro,
    // Named("forward") = forward,
    // Named("cheat") = cheat
  );
}
