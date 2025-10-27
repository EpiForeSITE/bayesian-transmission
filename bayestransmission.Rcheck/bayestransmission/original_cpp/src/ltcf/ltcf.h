
#ifndef _LTCF_DEFINED

	#define _LTCF_DEFINED
	
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
	#include "lognormal.h"

	namespace ltcf
	{
		#include "LTCFCoding.h"
		#include "LTCFLocationState.h"
		#include "LTCFPatientState.h"
		#include "LTCFInColParams.h"
		#include "LTCFModel.h"
		#include "readLTCF.h"
	}

	using namespace ltcf;

#endif

