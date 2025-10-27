
#ifndef _INFECT_DEFINED

	#define _INFECT_DEFINED
	
	#include <stdlib.h>
	#include <limits>
	#include <math.h>
	#include "util.h"
	#include <sstream>
	#include <exception>

	namespace infect
	{
		#include "EventCoding.h"
		#include "Patient.h"
		#include "RawEvent.h"
		#include "EpisodeEvent.h"
		#include "EventStack.h"
		#include "Episode.h"
		#include "RawEventList.h"
		#include "History.h"
		#include "Counter.h"
		#include "Model.h"
		#include "Likelihood.h"
		#include "EpisodeSampler.h"
		#include "ParameterSampler.h"
		#include "GibbsParameterSampler.h"
		#include "MetroParameterSampler.h"
	//	#include "ConstrainedParSampler.h"
	//	#include "LogLinearColSampler.h"
	}

	using namespace infect;

#endif
