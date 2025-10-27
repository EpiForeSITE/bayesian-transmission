
#ifndef _MODELS_DEFINED

	#define _MODELS_DEFINED
	
	#include <stdlib.h>
	#include <limits>
	#include <math.h>
	#include <sstream>
	#include <exception>
        #include <stdexcept>

        using namespace std;

	#include "util.h"
	#include "infect.h"

	namespace models
	{
	// Model parameter classes.

		#include "Parameters.h"
		#include "InsituParams.h"
		#include "TestParams.h"
		#include "TestParamsAbx.h"
		#include "RandomTestParams.h"
		#include "OutColParams.h"
		#include "InColParams.h"
		#include "MassActionICP.h"
		#include "AbxParams.h"

	// Models.
		#include "UnitLinkedModel.h"
		#include "ForwardSimulator.h"
		#include "ConstrainedSimulator.h"
		#include "BasicModel.h"
		#include "DummyModel.h"
		#include "MassActionModel.h"


	// Command line options handling.
		#include "Options.h"

	}

	using namespace models;

#endif
