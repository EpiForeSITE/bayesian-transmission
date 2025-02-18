
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

#include "modelsetup.h"
lognormal::LogNormalModel* newModel(
        std::string modname,
        int nstates,
        Rcpp::List modelParameters, //< Model specific options.
        bool verbose = false)
{
    lognormal::LogNormalModel *model = 0;

    if (modname == "LinearAbxModel")
    {
        model = new lognormal::LinearAbxModel(
            nstates,
            modelParameters["nmetro"],
            modelParameters["forward"],
            modelParameters["cheat"]
        );
    } else
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
    modelsetup(model, modelParameters, verbose);

    return model;
}


// [[Rcpp::export]]
SEXP runMCMC(
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

    if(verbose) Rcpp::Rcout << "Creating RNG...";

    RRandom *random = new RRandom();

    if(verbose) Rcpp::Rcout << "Done" << std::endl;

    if(verbose) Rcpp::Rcout << "Setting up System...";

    System *sys = new System(
        as<std::vector<int>>(data[0]),//faclity
        as<std::vector<int>>(data[1]),//unit
        as<std::vector<double>>(data[2]),//"times"
        as<std::vector<int>>(data[3]),//"patients"
        as<std::vector<int>>(data[4])//"type"
    );
    if(verbose)
        Rcpp::Rcout << "Done" << std::endl;


    //Model
    if(verbose)
        Rcpp::Rcout << "Creating model...";

    lognormal::LogNormalModel *model = newModel(modname, nstates, modelParameters, verbose);
    if(verbose)
        Rcpp::Rcout << "Done" << std::endl;

    // Set time origin of model.
    LogNormalICP *icp = (LogNormalICP *) model->getInColParams();
    icp->setTimeOrigin((sys->endTime()-sys->startTime())/2.0);
    if(verbose)
        Rcpp::Rcout << "Set time origin" << std::endl;


    // Create state history.

    if (verbose)
        Rcpp::Rcout << "Building history structure...";

    SystemHistory *hist = new SystemHistory(sys, model, false);
    if(verbose)
        Rcpp::Rcout << "Done" << std::endl;

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

    bool outputparam = MCMCParameters["outputparam"];
    unsigned int nsims = MCMCParameters["nsims"];

    Rcpp::List paramchain(nsims);
    Rcpp::NumericVector llchain(nsims);
    if (verbose)
        Rcpp::message(Rcpp::wrap(string("Building sampler.\n")));

    Sampler *mc = new Sampler(hist,model,random);

    if (verbose)
        Rcpp::message(Rcpp::wrap(string("burning in MCMC.\n")));
    unsigned int nburn = MCMCParameters["nburn"];
    for (unsigned int i=0; i<nburn; i++)
    {
        if(verbose) Rcout << i << ":sample episodes...";
        mc->sampleEpisodes();
        if(verbose) Rcout << "Sample Model...";
        mc->sampleModel();
        if(verbose) Rcout << "done." << std::endl;
    }

    if (verbose)
        Rcpp::message(Rcpp::wrap(string("Running MCMC.\n")));


    for (unsigned int i=0; i<nsims; i++)
    {
        if(verbose) Rcout << i << ":sample episodes...";
        mc->sampleEpisodes();
        if(verbose) Rcout << "Sample Model...";
        mc->sampleModel();

        if (outputparam)
        {
        //     cout << model << "\t\t" << model->logLikelihood(hist) << "\n";
        //     cout.flush();
            if (verbose)
                Rcout << "Outputting parameters...";
            paramchain(i) = model2R(model);
            if (verbose) Rcout << "likelhood...";
            llchain(i) = model->logLikelihood(hist);
        }
        if(verbose) Rcout << "done." << std::endl;
    }

    if (verbose)
        Rcpp::message(Rcpp::wrap(string("MCMC done.\n")));

    for (int j=0; j<wntests; j++)
    {
        HistoryLink *hh = histlink[j];
        double p = testtype[j]->eventProb(hh->getPState()->infectionStatus(),hh->getPState()->onAbx(),hh->getEvent()->getType());
        wprob += p;
        wlogprob += log(p);
        wlogsqprob += log(p)*log(p);
    }

    wprob /= wntests * nsims;
    wlogprob /= wntests * nsims;
    wlogsqprob /= wntests * nsims;
    double waic1 = 2*log(wprob) - 4*wlogprob;
    double waic2 = -2 * log(wprob) - 2 * wlogprob*wlogprob + 2 * wlogsqprob;
    if(verbose)
        Rcout << "WAIC 1 2 = \t" << waic1 << "\t" << waic2 << "\n";

/*
*/

    Rcpp::List ret = Rcpp::List::create(
        _["Parameters"] = paramchain,
        _["LogLikelihood"] = llchain,
        _["MCMCParameters"] = MCMCParameters,
        _["ModelParameters"] = modelParameters,
        _["ModelName"] = modname,
        _["nstates"] = nstates,
        _["waic1"] = waic1,
        _["waic2"] = waic2
    );

    bool outputfinal = MCMCParameters["outputfinal"];

    if(outputfinal)
    {
        if (verbose)
            Rcout << "Writing complete form of final state." << std::endl;

        ret["FinalModel"] = model2R(model);
    }
    delete [] histlink;
    delete [] testtype;
    delete tests;
    delete mc;
    delete hist;
    delete sys;
    delete model;
    delete random;
    delete AbxCoding::sysabx;

    return ret;

}
