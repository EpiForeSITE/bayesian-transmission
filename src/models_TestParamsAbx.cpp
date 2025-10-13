#include "modeling/modeling.h"

namespace models {
TestParamsAbx::TestParamsAbx(int nst, bool abx) : TestParams(nst)
{
    useabx = abx;

    nstates = nst;
    l = 3;
    m = 2;
    n = 2;

    probs = cleanAlloc(l,m,n);
    logprobs = cleanAlloc(l,m,n);
    counts = cleanAlloc(l,m,n);
    priors = cleanAlloc(l,m,n);
    doit = cleanAllocInt(l,m);

    set(0,0,0,0,0.5,2);
    set(1,0,0,0,0.5,2);
    set(2,0,0.8,1,0.5,2);

    set(0,1,0,0,0.5,2);
    set(1,1,0,0,0.5,2);
    set(2,1,0.8,1,0.5,2);

    initCounts();
}

TestParamsAbx::~TestParamsAbx()
{
    cleanFree(&probs,l,m);
    cleanFree(&logprobs,l,m);
    cleanFree(&counts,l,m);
    cleanFree(&priors,l,m);
    cleanFree(&doit,l);
}

std::vector<std::string> TestParamsAbx::paramNames() const
{
    std::vector<std::string> res(2*nstates);

    if (nstates == 3)
    {
        res[0] = "ATest.P(test+|unc,abx-)";
        res[1] = "ATest.P(test+|lat,abx-)";
        res[2] = "ATest.P(test+|col,abx-)";
        res[3] = "ATest.P(test+|unc,abx+)";
        res[4] = "ATest.P(test+|lat,abx+)";
        res[5] = "ATest.P(test+|col,abx+)";
    }

    if (nstates == 2)
    {
        res[0] = "ATest.P(test+|unc,abx-)";
        res[1] = "ATest.P(test+|col,abx-)";
        res[2] = "ATest.P(test+|unc,abx+)";
        res[3] = "ATest.P(test+|col,abx+)";
    }

    return res;
}

double TestParamsAbx::eventProb(InfectionStatus s, int onabx, EventCode e) const
{
    int i = stateIndex(s);
    int j = ( useabx && onabx ? 1 : 0) ;
    int k = testResultIndex(e);
    return ( i < 0 || k < 0 ? 0 : probs[i][j][k] );
}

double* TestParamsAbx::resultProbs(int onabx, EventCode e) const
{
    double *P = cleanAlloc(nstates);

    if (nstates == 2)
    {
        P[0] = eventProb(uncolonized,onabx,e);
        P[1] = eventProb(colonized,onabx,e);
    }

    if (nstates == 3)
    {
        P[0] = eventProb(uncolonized,onabx,e);
        P[1] = eventProb(latent,onabx,e);
        P[2] = eventProb(colonized,onabx,e);
    }

    return P;
}
double TestParamsAbx::logProb(infect::HistoryLink *const h) const
{
    int i = stateIndex(h->getPState()->infectionStatus());
    int j = ( useabx && h->getPState()->onAbx() ? 1 : 0) ;
    int k = testResultIndex(h->getEvent()->getType());

    return ( i < 0 || k < 0 ? 0 : logprobs[i][j][k] );
}

void TestParamsAbx::initCounts()
{
    for (int i=0; i<l; i++)
        for (int j=0; j<m; j++)
            for (int k=0; k<n; k++)
                counts[i][j][k] = priors[i][j][k];
}

void TestParamsAbx::count(infect::HistoryLink * const h)
{
    int i = stateIndex(h->getPState()->infectionStatus());
    int j = ( useabx && h->getPState()->onAbx() ? 1 : 0) ;
    int k = testResultIndex(h->getEvent()->getType());

    if (i >= 0 && k >= 0)
        counts[i][j][k] += 1;
}

void TestParamsAbx::update(Random *r, bool max)
{
    double **newpos = cleanAlloc(l,m);

    for (int i=0; i<l; i++)
        for (int j=0; j<m; j++)
        {
            if (doit[i][j])
            {
                if (max)
                    newpos[i][j] = (counts[i][j][1]-1) / (counts[i][j][1] + counts[i][j][0]-2);
                else
                    newpos[i][j] = r->rbeta(counts[i][j][1],counts[i][j][0]);
            }
            else
                newpos[i][j] = probs[i][j][1];
        }

    for (int i=0; i<l; i++)
        for (int j=0; j<m; j++)
            set(i,j,newpos[i][j]);

    cleanFree(&newpos,l);
}

// Personal accessors.

// Set value, update, and Beta priors.
void TestParamsAbx::set(unsigned int state, unsigned int abxstatus, double value, int update, double prival, double prin)
{
    if (value < 0 || value > 1)
    {
        cerr << "Can't set probability value outside (0,1)\t" << value << "\n";
        exit(1);
    }
    if (prival < 0 || prival > 1)
    {
        cerr << "Can't set probability prior value outside (0,1)\t" << prival << "\n";
        exit(1);
    }
    if (prin < 0)
    {
        cerr << "Can't set prior observation count negative\t" << prin << "\n";
        exit(1);
    }

    set(state,abxstatus,value);

    doit[state][abxstatus] = (update != 0);

    priors[state][abxstatus][0] = (1-prival)*prin;
    priors[state][abxstatus][1] = prival*prin;
}

int TestParamsAbx::nParam() const
{
    return 2*nstates;
}


void TestParamsAbx::write (ostream &os) const
{
    char *buffer = new char[100];

    sprintf(buffer,"%12.10f\t",probs[0][0][1]);
    os << buffer;
    if (nstates == 3)
    {
        sprintf(buffer,"%12.10f\t",probs[1][0][1]);
        os << buffer;
    }
    sprintf(buffer,"%12.10f",probs[2][0][1]);
    os << buffer;

    if (useabx)
    {
        sprintf(buffer,"\t%12.10f\t",probs[0][1][1]);
        os << buffer;
        if (nstates == 3)
        {
            sprintf(buffer,"%12.10f\t",probs[1][1][1]);
            os << buffer;
        }
        sprintf(buffer,"%12.10f",probs[2][1][1]);
        os << buffer;
    }

    delete [] buffer;
}

std::vector<double> TestParamsAbx::getValues() const
{
    // cout << "TestParamsAbx::getValues() called.\n";
    std::vector<double> res(2*nstates);

    if (nstates == 3)
    {
        res[0] = probs[0][0][1];//"ATest.P(test+|unc,abx-)";
        res[1] = probs[1][0][1];//"ATest.P(test+|col,abx-)";
        res[2] = probs[2][0][1];//"ATest.P(test+|lat,abx-)";
        res[3] = probs[0][1][1];//"ATest.P(test+|unc,abx+)";
        res[4] = probs[1][1][1];//"ATest.P(test+|lat,abx+)";
        res[5] = probs[2][1][1];//"ATest.P(test+|col,abx+)";
    }

    if (nstates == 2)
    {
        res[0] = probs[0][0][1];//"ATest.P(test+|unc,abx-)";
        res[1] = probs[2][0][1];//"ATest.P(test+|col,abx-)";
        res[2] = probs[0][1][1];//"ATest.P(test+|unc,abx-)";
        res[3] = probs[2][1][1];//"ATest.P(test+|col,abx+)";
    }

    return res;

}

} // namespace models
