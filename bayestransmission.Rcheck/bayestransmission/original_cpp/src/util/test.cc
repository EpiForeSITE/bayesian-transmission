
#include <stdio.h>
#include <iostream>
#include "util.h"

int main(int argc, char *argv[])
{
/*
	for (int i=-5; i <=15; i++)
	{
		double x = i/10.0;
		cout << x << "\t" << logit(x) << "\t" << logistic(logit(x)) << "\n";
	}
*/
	for (double x = 0.1; x < 10; x += 0.1)
	{
		cout << x << "\t" << digamma(x) << "\t" << trigamma(x) << "\n";
	}
}


