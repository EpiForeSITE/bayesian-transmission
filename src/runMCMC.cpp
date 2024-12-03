
#include <string>
using std::string;

#include "infect/infect.h"
using namespace infect;
using namespace util;

#include <Rcpp.h>
using namespace Rcpp;

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



  return DataFrame::create(
    Named("verbose") = verbose,
    Named("nmetro") = nmetro,
    Named("forward") = forward,
    Named("cheat") = cheat
  );
}

/*
void SetInSituParameters(InsituParams *isp, Rcpp::List params)
{
    double p,q,r;
    int up, uq, ur;

    if(params.containsElementNamed("p")){
        p = Rcpp::as<double>(idf["p"]);
        up = Rcpp::as<int>(idf["p"]);
    }
    if(params.containsElementNamed("q")){
        q = Rcpp::as<double>(idf["q"]);
        uq = Rcpp::as<int>(idf["q"]);
    }
    if(params.containsElementNamed("r")){
        r = Rcpp::as<double>(idf["r"]);
        ur = Rcpp::as<int>(idf["r"]);
    }

    p = p+q+r;
    r = r/p;
    q = q/p;
    p = 1-q-r;
    up = ur;

    isp->set(p,q,r);
    isp->setPriors(p*up,q*up,r*ur);
    isp->setUpdate(up,uq,ur);
}

void SetSurveillanceTestParameters(TestParams *stsp, Rcpp::List params, int nstates)
{
    double p, q, r;
    int up, uq, ur;

    if(idf.containsElementNamed("p")){
        p = Rcpp::as<double>(idf["p"]);
    }
    if(idf.containsElementNamed("up")){
        up = Rcpp::as<int>(idf["up"]);
    }

    if(nstates == 3){
        if(idf.containsElementNamed("q")){
            q = Rcpp::as<double>(idf["q"]);
        }
        if(idf.containsElementNamed("uq")){
            uq = Rcpp::as<int>(idf["uq"]);
        }
    } else {
        q = 0;
        uq = 0;
    }

    if(idf.containsElementNamed("r")){
        r = Rcpp::as<double>(idf["r"]);
    }
    if(idf.containsElementNamed("ur")){
        ur = Rcpp::as<int>(idf["ur"]);
    }



    stsp->set(0,p,q,r);
    stsp->setPriors(0,p,up,q,uq,r,ur);
    stsp->setUpdate(0,up,uq,ur);
}
void SetClinicalTestParameters(RandomTestParams *ctsp, Rcpp::List params, int nstates)
{
    double p, q, r;
    int up, uq, ur;

    if(idf.containsElementNamed("p")){
        p = Rcpp::as<double>(idf["p"]);
    }
    if(idf.containsElementNamed("up")){
        up = Rcpp::as<int>(idf["up"]);
    }

    if(nstates==3){
        if(idf.containsElementNamed("q")){
            q = Rcpp::as<double>(idf["q"]);
        }
        if(idf.containsElementNamed("uq")){
            uq = Rcpp::as<int>(idf["uq"]);
        }
    } else {
        q = 0;
        uq = 0;
    }
    if(idf.containsElementNamed("r")){
        r = Rcpp::as<double>(idf["r"]);
    }
    if(idf.containsElementNamed("ur")){
        ur = Rcpp::as<int>(idf["ur"]);
    }



    if(nstates==3){
        if(idf.containsElementNamed("q2")){
            q = Rcpp::as<double>(idf["q2"]);
        }
        if(idf.containsElementNamed("uq2")){
            uq = Rcpp::as<int>(idf["uq2"]);
        }
    } else {
        q=1;
        uq=0;
    }

    if(idf.containsElementNamed("r2")){
        r = Rcpp::as<double>(idf["r2"]);
    }
    if(idf.containsElementNamed("ur2")){
        ur = Rcpp::as<int>(idf["ur2"]);
    }

    ctsp->set(p,q,r);
    ctsp->setPriors(p,up,q,uq,r,ur);
    ctsp->setUpdate(up,uq,ur);


    ctsp->setRates(p,q,r);
    ctsp->setUpdateRates(up,uq,ur);
}


MixedModel ReadModelFromR(Rcpp::List idf, int nmetro, bool forward, bool cheat)
{

    int nstates = 0;
    double abxd = 0.0;
    double abxl = 0.0;

    if(idf.containsElementNamed("nstates"))
        nstates = Rcpp::as<int>(idf["nstates"]);
    if(idf.containsElementNamed("abxd"))
        abxd = Rcpp::as<double>(idf["abxd"]);
    if(idf.containsElementNamed("abxl"))
        abxl = Rcpp::as<double>(idf["abxl"]);

    switch(nstates)
    {
    case 2:
    case 3:
        break;
    default:
        throw std::invalid_argument("nstates must be 2 or 3");
    }

    MixedModel *model = new MixedModel(nstates,nmetro,forward,cheat);

    model->setAbxDelay(abxd);
    model->setAbxLife(abxl);

    // In situ parameters.
    SetInSituParameters(model->getInSituParams(), idf["insitu"]);




    return model;
}
*/
