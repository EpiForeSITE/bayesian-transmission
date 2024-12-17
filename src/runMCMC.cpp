
#include <string>
using std::string;

#include "util/util.h"
#include "infect/infect.h"
#include "modeling/modeling.h"
#include "lognormal/lognormal.h"

using namespace infect;
using namespace util;

#include <Rcpp.h>
using namespace Rcpp;

#include "RRandom.h"

lognormal::LogNormalModel* newModel(std::string modname, int nstates, int nmetro, bool forward, bool cheat = false)
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
        model = new lognormal::MixedModel(nstates,nmetro,forward,cheat);
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
        model = new lognormal::LogNormalModel(nstates,nmetro,forward,cheat);
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
    std::string modoptions,
    Rcpp::DataFrame data,
    int nsims,
    int nburn = 0,
    int nmetro = 10,
    bool forward = false,
    bool cheat = false,
    bool outputfinal = false,
    bool outputparam = true,
    bool verbose = false
) {
    if(verbose)
        Rcpp::message(Rcpp::wrap(string("Initializing Variables")));

    System *sys = new System(data["facilities"], data["units"], data["times"], data["patients"], data["types"]);

    //Model
    //LogNormalModel *model = RMixedModel


  return DataFrame::create(
    Named("verbose") = verbose,
    Named("nmetro") = nmetro,
    Named("forward") = forward,
    Named("cheat") = cheat
  );
}
