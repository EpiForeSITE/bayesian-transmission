
	LTCFModel *readLTCF(istream &is, List *l, int nmetro, int forward, int cheat)
	{
		string modname;
		int nstates;

	      	is >> modname >> nstates;
		char c;
		do { c = is.get(); } while (c != '\n');


                LTCFModel *model = 0;

		if (modname == "LTCFModel")
		{
			model = new LTCFModel(nstates,nmetro,forward,cheat);
		}
                else
                {
                        cerr << "Model type: " << modname << " " << nstates << " is not recognized\n";
                        exit(1);
                }

                model->read(is);

		return model;
	}
