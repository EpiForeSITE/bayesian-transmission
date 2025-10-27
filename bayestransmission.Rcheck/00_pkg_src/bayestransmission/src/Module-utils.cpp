
#include "modeling/modeling.h"
using namespace models;


#include "wrap.h"

#include <Rcpp.h>

#include "RRandom.h"

// Expose the classes to R

void init_Module_util(){
class_<util::Object>("CppObject")
    .property("hash", &util::Object::hash)
    .property("className", &util::Object::className)
    ;
class_<util::Map>("CppMap")
    .derives<util::Object>("CppObject")
    .property("size", &util::Map::size)
    .property("hasNext", &util::Map::hasNext)
    .method("init", &util::Map::init)
    .method("step", &util::Map::next)
    .method("nextValue", static_cast<util::Object* (util::Map::*)()>(&util::Map::nextValue))
    .method("FirstKey", &util::Map::getFirstKey)
    .method("FirstValue", &util::Map::getFirstValue)
    .method("LastKey", &util::Map::getLastKey)
    .method("LastValue", &util::Map::getLastValue)
    ;



}
