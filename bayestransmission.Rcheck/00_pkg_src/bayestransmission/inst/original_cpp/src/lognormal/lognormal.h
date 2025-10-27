
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
		#include "LNMassActionICP.h"

		#include "LogNormalAbxICP.h"
		#include "MixedICP.h"
		#include "LinearAbxICP.h"
		#include "LinearAbxICP2.h"

		#include "LogNormalModel.h"
		#include "LNMassActionModel.h"
		#include "LogNormalAbxModel.h"
		#include "MixedModel.h"
		#include "LinearAbxModel.h"
		#include "LinearAbxModel2.h"

		#include "MultiUnitLinearAbxICP.h"
		#include "MultiUnitLinearAbxModel.h"

		#include "readModel.h"
	}

	using namespace lognormal;


#endif

