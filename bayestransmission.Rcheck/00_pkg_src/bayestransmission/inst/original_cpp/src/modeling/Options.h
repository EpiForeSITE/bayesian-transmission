
class Options : public Object
{
public:

	int nstates;
	int clinical;
	int densityModel;
	int seed;
	int nburn;
	int nsims;
	int nmetro;
	int allout;
	int verbose;
	int errors;

	Options(int argc, char *argv[])
	{
		nstates = 2;
		clinical = 0;
		densityModel = 1;
		seed = 1;
		nburn = 0;
		nsims = 1000;
		nmetro = 10;
		verbose = 0;
		allout = 0;
		errors = 0;

		switch(argc)
		{
		case 10: sscanf(argv[9],"%d",&verbose);
		case 9: sscanf(argv[8],"%d",&allout);
		case 8: sscanf(argv[7],"%d",&nmetro);
		case 7: sscanf(argv[6],"%d",&nsims);
		case 6: sscanf(argv[5],"%d",&nburn);
		case 5: sscanf(argv[4],"%d",&seed);
		case 4: sscanf(argv[3],"%d",&densityModel);
		case 3: sscanf(argv[2],"%d",&clinical);
		case 2: sscanf(argv[1],"%d",&nstates);
		}
	
		switch(nstates)
		{
		case 2:
		case 3:
			break;
		default:
			errors++;
		}
	
		switch(clinical)
		{
		case 0:
		case 1:
			break;
		default:
			errors++;
		}
	
		switch(densityModel)
		{
		case 0:
		case 1:
		case 2:
			break;
		default:
			errors++;
		}
	
		if (seed <= 0)
			seed = 1;
		
		if (nburn < 0)
			nburn = 0;
	
		if (nsims < 0)
			nsims = 1000;
	
		if (nmetro < 0)
			nmetro = 10;
	
		if (allout < 0)
			allout = 0;
	
		if (verbose < 0)
			verbose = 0;
	}
};
