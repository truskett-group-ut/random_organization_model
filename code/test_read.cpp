#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <deque>
#include <queue>
#include <stack>
#include <string>
#include <bitset>
#include <cstdio>
#include <limits>
#include <vector>
#include <climits>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <numeric>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <tuple>
#include <array>
#include <random>
#include <regex>
#include <set>
#include <omp.h> 
#include <chrono>

using namespace std;

struct SimData{
    double hours_prod, eta_begin, eta_end, eta_inc, drmax;
    long long int steps_prod;
    int skip_steps, skip_steps_traj, skip_steps_check, seed, nparticle;
    vector< double > etas;
   
    void Initialize(double eta_begin, double eta_end, double eta_inc){
        for (double n = eta_begin; n <= eta_end; n+=eta_inc) {
            etas.push_back(n);
        } 
    }

}

//function definitions
SimData ReadSimulationData();

int main(){
        SimData simdata=ReadSimulationData();
        simdata.Initialize(simdata.eta_begin, simdata.eta_end, simdata.eta_inc);
        for (unsigned i=0; i<simdata.etas.size(); i++) {
            cout << simdata.etas[i] << "\n"; }
        return 0;
}

SimData ReadSimulationData(){

        vector<string> list;
        string entry;

        ifstream infile("simulation.txt");
	if (!infile) { cerr << "Unable to open simulation.txt\n"; exit(1); }

        while (infile >> entry) {
          list.push_back(entry);
        }
        infile.close();
         
        for (unsigned n=0; n<list.size(); n++) {
            cout << list.at(n) << "\n";
        }
        
        vector<string> keywords;
        regex kw_re;
	smatch match;
        map<string, string> simdatamap;
        
        keywords.push_back("steps_prod");
        keywords.push_back("hours_prod");
        keywords.push_back("skip_steps");
        keywords.push_back("skip_steps_traj");
        keywords.push_back("skip_steps_check");
        keywords.push_back("dr_max");
        keywords.push_back("eta_begin");
        keywords.push_back("eta_end");
        keywords.push_back("eta_inc");
        keywords.push_back("seed");
        keywords.push_back("nparticle");

        for (unsigned i=0; i<keywords.size(); i++) 
        {
            kw_re.assign(keywords[i]+"\\s*\\=\\s*"+"([0-9\\.\\-]+)");
            for (unsigned n=0; n<list.size(); n++) 
            {
              if ( regex_search(list[n], match, kw_re) )
              {
                simdatamap[keywords[i]] = match.str(1);
              }
            }
        }
        
        SimData simdata;
        simdata.eta_begin=stod(simdatamap["eta_begin"]);
        simdata.eta_inc=stod(simdatamap["eta_inc"]);
        simdata.eta_end=stod(simdatamap["eta_end"]);
        simdata.steps_prod=stoi(simdatamap["steps_prod"]);
        simdata.hours_prod=stod(simdatamap["hours_prod"]);
        simdata.skip_steps=stoi(simdatamap["skip_steps"]);
        simdata.skip_steps_traj=stoi(simdatamap["skip_steps_traj"]);
        simdata.skip_steps_check=stoi(simdatamap["skip_steps_check"]);
        simdata.drmax=stod(simdatamap["drmax"]);
        simdata.seed=stoi(simdatamap["seed"]);
        simdata.nparticle=stoi(simdatamap["nparticle"]);
        return simdata;
}

