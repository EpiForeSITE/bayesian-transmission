
	LogNormalModel *readModel(istream &is, List *l, int nmetro, int forward, int cheat)
	{
		string modname;
		int nstates;

	      	is >> modname >> nstates;
		char c;
		do { c = is.get(); } while (c != '\n');


                LogNormalModel *model = 0;

                if (modname == "LinearAbxModel")
                {
                        model = new LinearAbxModel(nstates,nmetro,forward,cheat);
                }
                else if (modname == "LinearAbxModel2")
                {
                        model = new LinearAbxModel2(nstates,nmetro,forward,cheat);
                }
		else if (modname == "MultiUnitLinearAbxModel")
		{
                        model = new MultiUnitLinearAbxModel(nstates,l,nmetro,forward,cheat);
		}
                else if (modname == "MixedModel")
                {
                        model = new MixedModel(nstates,nmetro,forward,cheat);
                }
                else if (modname == "LogNormalAbxModel")
                {
                        model = new LogNormalAbxModel(nstates,nmetro,forward,cheat);
                }
		else if (modname == "LNMassActionModel")
		{
			model = new LNMassActionModel(nstates,nmetro,forward,cheat);
		}
		else if (modname == "LogNormalModel")
		{
			model = new LogNormalModel(nstates,nmetro,forward,cheat);
		}
                else
                {
                        cerr << "Model type: " << modname << " " << nstates << " is not recognized\n";
                        exit(1);
                }

                model->read(is);

		return model;
	}
