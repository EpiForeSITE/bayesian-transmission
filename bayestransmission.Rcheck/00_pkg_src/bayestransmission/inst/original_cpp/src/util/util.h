
#ifndef _UTIL_DEFINED

	#define _UTIL_DEFINED

	#include <stdio.h>
	#include <iostream>
	#include <stdlib.h>
	#include <math.h>
	#include <stdexcept>
	#include <vector>
	using namespace std;

	namespace util
	{
		#include "Allocator.h"
		#include "Object.h"
		#include "Random.h"
		#include "Integer.h"
		#include "Array.h"
		#include "Vector.h"
		#include "Map.h"
		#include "IntMap.h"
		#include "List.h"
		#include "SortedList.h"
		#include "GaussianProcess.h"
		#include "Markov.h"

		double digamma(double x);
		double trigamma(double x);
		double lbeta(double a, double b);
		double logit(double x);
		double logistic(double x);
		double logint(int x);

		void sanitize();
	}

	using namespace util;
#endif
