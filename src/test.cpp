#include <Rcpp.h>
using namespace Rcpp;

// [[Rcpp::export]]
void hello_world() {
  Rcout << "Hello, world!" << std::endl;
}
