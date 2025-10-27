#include <stdio.h>
#include <iostream>
#include <sstream>

#include "infect.h"

int main(int argc, char *argv[])
{
	try
	{
		int quit = 0;
		int maxcode = 1000;
		int maxline = 1000;
		char *c = new char[maxline];
		string sdump;

		// Input event coding.

		int *c2s = new int[maxcode];
		for (int i=0; i<maxcode; i++)
			c2s[i] = -1;

		int *s2c = new int[EventCoding::maxeventcode];
		for (int i=0; i<EventCoding::maxeventcode; i++)
			s2c[i] = -1;

		for (cin.getline(c,maxline); c[0] == '#'; cin.getline(c,maxline))
		{
			string event;
			int code;
			stringstream s(c,stringstream::in);
			s >> sdump >> event >> code;

			if (code < 0 || code >= maxcode)
			{
				cerr << "Event code out of range [0," << maxcode << ":\t" << event << "\t" << code << "\n";
				quit = 1;
			}
			else if (event == "admission")
			{
				s2c[EventCoding::admission] = code;
				c2s[code] = EventCoding::admission;
			}
			else if (event == "discharge")
			{
				s2c[EventCoding::discharge] = code;
				c2s[code] = EventCoding::discharge;
			}
			else if (event == "postest")
			{
				s2c[EventCoding::postest] = code;
				c2s[code] = EventCoding::postest;
			}
			else if (event == "negtest")
			{
				s2c[EventCoding::negtest] = code;
				c2s[code] = EventCoding::negtest;
			}
			else if (event == "posclintest")
			{
				s2c[EventCoding::posclintest] = code;
				c2s[code] = EventCoding::posclintest;
			}
			else if (event == "negclintest")
			{
				s2c[EventCoding::negclintest] = code;
				c2s[code] = EventCoding::negclintest;
			}
			else if (event == "possurvtest")
			{
				s2c[EventCoding::possurvtest] = code;
				c2s[code] = EventCoding::possurvtest;
			}
			else if (event == "negsurvtest")
			{
				s2c[EventCoding::negsurvtest] = code;
				c2s[code] = EventCoding::negsurvtest;
			}
			else if (event == "abxon")
			{
				s2c[EventCoding::abxon] = code;
				c2s[code] = EventCoding::abxon;
			}
			else if (event == "abxoff")
			{
				s2c[EventCoding::abxoff] = code;
				c2s[code] = EventCoding::abxoff;
			}
		}

		// Check event coding.

		cerr << "The following events and codes were recognized.\n";
		for (int i=0; i<EventCoding::maxeventcode; i++)
		{
			if (s2c[i] >= 0)
			{
				cerr << EventCoding::eventString(i) << "\t" << s2c[i] << "\n";
			}
		}

		if (s2c[EventCoding::admission] < 0)
		{
			cerr << "No code specified for admissions.\n";
			quit = 1;
		}

		if (s2c[EventCoding::discharge] < 0)
		{
			cerr << "No code specified for discharges.\n";
			quit = 1;
		}
		
		// Find data columns. 

		int ig = -1;
		int iu = -1;
		int it = -1;
		int ip = -1;
		int ie = -1;

		stringstream s(c,stringstream::in);

		for (int i=0; !s.eof(); i++)
		{
			string w;
			s >> w;
			if (w == "facility")
				ig = i; 
			else if (w == "unit")
				iu = i;
			else if (w == "time")
				it = i;
			else if (w == "patient")
				ip = i;
			else if (w == "type")
				ie = i;
		}

		// Check data columns.

		if (it < 0)
		{
			cerr << "Time column not specified.\n";
			quit = 1;
		}

		if (ip < 0)
		{
			cerr << "Patient column not specified.\n";
			quit = 1;
		}
	
		if (ie < 0)
		{
			cerr << "Type column not specified.\n";
			quit = 1;
		}


		if (quit)
		{
			cerr << "Cannot reformat event list.\n";
			cerr << "Exiting.\n";
			exit(1);
		}

		if (iu < 0)
		{
			cerr << "Unit column not specified. ";
			cerr << "Will asign all events to unit 1.\n";
		}

	
		if (ig < 0)
		{
			cerr << "Facility column not specified. ";
			cerr << "Will asign all events to facility 1.\n";
		}

		cerr << "Will read data from following columns.\n";
		if (ig >= 0)
			cerr << "facility\t" << ig << "\n";
		if (iu >= 0)
			cerr << "unit    \t" << iu << "\n";
		cerr << "time    \t" << it << "\n";
		cerr << "patient  \t" << ip << "\n";
		cerr << "type    \t" << ie << "\n";


		// Read data and reformat in standard order.
	

		for (cin.getline(c,maxline) ; !cin.eof(); cin.getline(c,maxline) )
		{
			if (c[0] == 0)
				continue;
			stringstream ss(c,stringstream::in);

			string sg = "1";
			string su = "1";
			string st = "";
			string sp = "";
			int se = -1;

			for (int i=0; !ss.eof(); i++)
			{
				if (i == ig)
					ss >> sg;
				else if (i == iu)
					ss >> su;
				else if (i == it)
					ss >> st;
				else if (i == ip)
					ss >> sp;
				else if (i == ie)
					ss >> se;
				else
					ss >> sdump;
			}

			if (se < 0 || se >= maxcode || c2s[se] < 0)
			{
				cerr << "Unrecognized event code:\t"; 
				cerr << se << ":\t";
				cerr << c << "\n";
			}
			else
			{
				cout << sg << "\t" << su << "\t" << st << "\t" << sp << "\t" << c2s[se] << "\n";
			}
		}

		// Clean up.

		delete [] c;
		delete [] c2s;
		delete [] s2c;
	}
	catch (exception *ex)
	{
		cerr << "Exception:\n\n";
		cerr << ex->what();
	}
}
