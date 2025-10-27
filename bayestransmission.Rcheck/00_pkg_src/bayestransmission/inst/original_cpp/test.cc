
#include <stdio.h>
#include <iostream>
#include "util.h"

int main(int argc, char *argv[])
{
	for (double x = 1; x <= 100; x += 1)
	{
		cout << x << "\t" << digamma(x) << "\t" << trigamma(x) << "\n";
	}
}


