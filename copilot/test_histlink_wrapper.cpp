// Simple wrapper to test getHistoryLinkLogLikelihoods
// Add this to wrap.cpp

#include "wrap.h"

// [[Rcpp::export]]
Rcpp::List testHistoryLinkLogLikelihoods(Rcpp::List modelParameters) {
    
    bool verbose = true;
    Random *random = new RRandom();
    infect::System *sys = list2System(modelParameters["Data"], verbose);
    infect::SystemHistory *hist = new infect::SystemHistory(sys);
    models::BasicModel *model = NULL;
    
    std::string modelName = Rcpp::as<std::string>(modelParameters["modname"]);
    
    if (modelName == "LinearAbxModel2" || modelName == "LinearAbxModel") {
        model = buildLinearAbxModel2(modelParameters, sys, hist, random);
    } else if (modelName == "LogNormalModel") {
        model = buildLogNormalModel(modelParameters, sys, hist, random);
    } else {
        Rcpp::stop("Unknown model name: " + modelName);
    }
    
    if (verbose) Rcpp::Rcout << "Calling getHistoryLinkLogLikelihoods..." << std::endl;
    
    std::vector<double> lls = model->getHistoryLinkLogLikelihoods(hist);
    
    double overall_ll = model->logLikelihood(hist);
    
    delete model;
    delete hist;
    delete sys;
    delete random;
    
    return Rcpp::List::create(
        Rcpp::Named("linkLogLikelihoods") = lls,
        Rcpp::Named("overallLogLikelihood") = overall_ll,
        Rcpp::Named("numLinks") = lls.size()
    );
}
