
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
        Rcpp::List modelParameters, //< Model specific options.
        bool verbose = false)
{
    lognormal::LogNormalModel *model = 0;
    std::string modname = modelParameters["modname"];
    int nstates = modelParameters["nstates"];

    if (modname == "LinearAbxModel")
    {
        model = new LinearAbxModel(
            nstates,
            modelParameters["nmetro"],
            modelParameters["forward"],
            modelParameters["cheat"]
        );
        modelsetup<LinearAbxModel>((LinearAbxModel*)model, modelParameters, verbose);
    } else
    if (modname == "LinearAbxModel2")
    {
        model = new LinearAbxModel2(
            nstates,
            modelParameters["nmetro"],
            modelParameters["forward"],
            modelParameters["cheat"]
        );
        modelsetup<LinearAbxModel2>((LinearAbxModel2*)model, modelParameters, verbose);
    } else
    // if (modname == "MultiUnitLinearAbxModel")
    // {
    //     model = new lognormal::MultiUnitLinearAbxModel(nstates,l,nmetro,forward,cheat);
    // } else
    if (modname == "MixedModel")
    {
        model = new MixedModel(
            nstates,
            modelParameters["nmetro"],
            modelParameters["forward"],
            modelParameters["cheat"]
        );
        modelsetup<MixedModel>((MixedModel*)model, modelParameters, verbose);
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
        model = new LogNormalModel(
            nstates,
            modelParameters["nmetro"],
            modelParameters["forward"],
            modelParameters["cheat"]
        );
        modelsetup<LogNormalModel>((LogNormalModel*)model, modelParameters, verbose);
    }
    else
    {
        throw std::invalid_argument("Invalid model name");
    }

    //model->setup(modOptions);
    //modelsetup(model, modelParameters, verbose);

    return model;
}


//' Run Bayesian Transmission MCMC
//'
//' @param modname Name of the model to run.
//' @param data Data frame with columns, in order: facility, unit, time, patient, and event type.
//' @param MCMCParameters List of MCMC parameters.
//' @param modelParameters List of model parameters, see <LogNormalModelParams>.
//' @param nstates Number of states in the model.
//' @param verbose Print progress messages.
//'
//' @return A list with the following elements:
//'   * `Parameters` the MCMC chain of model parameters
//'   * `LogLikelihood` the log likelihood of the model at each iteration
//'   * `MCMCParameters` the MCMC parameters used
//'   * `ModelParameters` the model parameters used
//'   * `ModelName` the name of the model
//'   * `nstates` the number of states in the model
//'   * `waic1` the WAIC1 estimate
//'   * `waic2` the WAIC2 estimate
//'   * and optionally (if `MCMCParameters$outputfinal` is true) `FinalModel` the final model state.
//' @export
// [[Rcpp::export]]
SEXP runMCMC(
    Rcpp::DataFrame data,
    Rcpp::List MCMCParameters,
    Rcpp::List modelParameters,
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
        as<std::vector<int>>(data[0]),//facility
        as<std::vector<int>>(data[1]),//unit
        as<std::vector<double>>(data[2]),//"time"
        as<std::vector<int>>(data[3]),//"patient"
        as<std::vector<int>>(data[4])//"event type"
    );
    if (verbose) Rcpp::Rcout << "Done" << std::endl;


    //Model
    if (verbose) Rcpp::Rcout << "Creating model...";

    lognormal::LogNormalModel *model = newModel(modelParameters, verbose);
    if (verbose) Rcpp::Rcout << "Done" << std::endl;

    // Set time origin of model.
    LogNormalICP *icp = (LogNormalICP *) model->getInColParams();
    icp->setTimeOrigin((sys->endTime()-sys->startTime())/2.0);
    if (verbose) Rcpp::Rcout << "Set time origin" << std::endl;


    // Create state history.

    if (verbose) Rcpp::Rcout << "Building history structure...";

    SystemHistory *hist = new SystemHistory(sys, model, false);
    if (verbose) Rcpp::Rcout << "Done" << std::endl;

    // Find tests for posterior prediction and, hence, WAIC estimates.

    if (verbose) Rcpp::message(Rcpp::wrap(string("Finding tests for WAIC.\n")));

    util::List* tests = hist->getTestLinks();
    TestParams** testtype = new TestParams*[tests->size()];
    HistoryLink** histlink = new HistoryLink*[tests->size()];

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
            testtype[wntests] = model->getSurveillanceTestParams();
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
    {
        Rcpp::Rcout << "\n=== INITIAL PARAMETERS ===" << std::endl;
        std::ostringstream ss;
        
        // Output parameter components
        model->getInsituParams()->write(ss);
        ss << "\t";
        model->getSurveillanceTestParams()->write(ss);
        ss << "\t";
        if (model->getClinicalTestParams() != model->getSurveillanceTestParams())
        {
            model->getClinicalTestParams()->write(ss);
            ss << "\t";
        }
        model->getOutColParams()->write(ss);
        ss << "\t";
        model->getInColParams()->write(ss);
        ss << "\t";
        if (model->getAbxParams() != 0)
        {
            model->getAbxParams()->write(ss);
            ss << "\t";
        }
        
        Rcpp::Rcout << ss.str() << "\t\tLogLike=" << model->logLikelihood(hist) << std::endl;
        Rcpp::Rcout << "=== END INITIAL PARAMETERS ===\n" << std::endl;
    }

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
    if (verbose) Rcout << "WAIC 1 2 = \t" << waic1 << "\t" << waic2 << "\n";

/*
*/

    Rcpp::List ret = Rcpp::List::create(
        _["Parameters"] = paramchain,
        _["LogLikelihood"] = llchain,
        _["MCMCParameters"] = MCMCParameters,
        _["ModelParameters"] = modelParameters,
        // _["ModelName"] = modname,
        // _["nstates"] = nstates,
        _["waic1"] = waic1,
        _["waic2"] = waic2
    );

    bool outputfinal = MCMCParameters["outputfinal"];

    if(outputfinal)
    {
        if (verbose) Rcout << "Writing complete form of final state." << std::endl;

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

//' Create a new model object
//'
//' Creates and initializes a model object based on the provided parameters.
//' This allows direct creation and inspection of model objects without running MCMC.
//' Returns a list with all model parameter values for verification.
//'
//' @param modelParameters List of model parameters, including:
//'   * `modname` Name of the model (e.g., "LogNormalModel", "LinearAbxModel", "LinearAbxModel2", "MixedModel")
//'   * `nstates` Number of states in the model
//'   * `nmetro` Number of metropolis steps
//'   * `forward` Forward parameter
//'   * `cheat` Cheat parameter
//' @param verbose Print progress messages (default: false)
//'
//' @return A list containing the initialized model parameters:
//'   * `Insitu` - In situ parameters
//'   * `SurveillanceTest` - Surveillance test parameters
//'   * `ClinicalTest` - Clinical test parameters
//'   * `OutCol` - Out of unit colonization parameters
//'   * `InCol` - In unit colonization parameters
//'   * `Abx` - Antibiotic parameters
//' @export
// [[Rcpp::export]]
SEXP newModelExport(
    Rcpp::List modelParameters,
    bool verbose = false
) {
    lognormal::LogNormalModel *model = newModel(modelParameters, verbose);
    
    // Use the existing model2R function to wrap the model parameters
    SEXP result = model2R(model);
    
    // Clean up the model object
    delete model;
    
    return result;
}
