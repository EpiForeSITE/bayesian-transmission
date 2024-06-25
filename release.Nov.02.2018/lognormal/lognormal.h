
#ifndef _LOGNORMAL_DEFINED

	#define _LOGNORMAL_DEFINED
	
	#include <stdlib.h>
	#include <limits>
	#include <math.h>
	#include <sstream>
	#include <exception>
        #include <stdexcept>

        using namespace std;

	#include "util.h"
	#include "infect.h"
	#include "modeling.h"

	namespace lognormal
	{
		#include "LogNormalICP.h"
		#include "LogNormalMassAct.h"
		#include "LogNormalAbxICP.h"
		#include "MultiUnitAbxICP.h"
		#include "MixedICP.h"

		#include "LogNormalModel.h"
		#include "MixedModel.h"
	}

	using namespace lognormal;

#endif
