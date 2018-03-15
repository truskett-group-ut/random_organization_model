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
//#include <windows.h>  

//volatile DWORD dwStart;

using namespace std;

const double pi = 3.1415926535897;

//convenient object to describe a particle
struct Particle {
	double rx, ry; 
	int type;
	int index;
};

//convenient object to store the details of the simulation
struct State {
	//explicitly loaded in
	int N; //number of particles
	vector< Particle > particles; //stores the particles
	//created via initialization function
	double eta;
	double diameter;
	double L; //box edge length

	//does any initialization needed after loading in relevant details
	void Initialize(){
		//calculate the box area/length
		L = sqrt(N*pi*pow(diameter,2) / eta)
	}
};


//reads in the intial details like volume fraction and particle composition
void LoadSimulationData(
	long long int &steps_equil, double &hours_equil, double &eta_inc
	long long int &steps_prod, double &hours_prod, 
	int &skip_steps, int &skip_steps_traj, long long int &skip_steps_check, double &dr_max, double &eta_begin, double &eta_end, int &seed){

        vector<string> list;
        string entry;

//	ifstream in_file;
//	in_file.open("./simulation.txt");
//	if (!in_file) { cerr << "Unable to open simulation.txt"; exit(1); }
        ifstream wordfile("simulation.txt");
        while (infile >> entry) {
          list.push_back(word);
        }
        

	//prepare the regex's and match
	regex num_re("([0-9\\.\\-]+)");
	smatch match;
	
	//read in prod details
	getline(in_file, line);
	regex_search(line, match, num_re);
	steps_prod = stoll(match.str(1));
	getline(in_file, line);
	regex_search(line, match, num_re);
	hours_prod = stod(match.str(1));

	//read in skip_steps
	getline(in_file, line);
	regex_search(line, match, num_re);
	skip_steps = stoi(match.str(1));

	//read in skip_steps_traj
	getline(in_file, line);
	regex_search(line, match, num_re);
	skip_steps_traj = stoi(match.str(1));

	//read in skip_steps_check
	getline(in_file, line);
	regex_search(line, match, num_re);
	skip_steps_check = stoll(match.str(1));

	//read in dr_max
	getline(in_file, line);
	regex_search(line, match, num_re);
	dr_max = stod(match.str(1));

	//read in first eta
	getline(in_file, line);
	regex_search(line, match, num_re);
	eta_begin = stod(match.str(1));

	//read in last eta
	getline(in_file, line);
	regex_search(line, match, num_re);
        eta_end = stod(match.str(1));

	//read in eta increment
	getline(in_file, line);
	regex_search(line, match, num_re);
        eta_end = stod(match.str(1));

	//read in seed
	getline(in_file, line);
	regex_search(line, match, num_re);
	seed = stoi(match.str(1));

	//close the file
	in_file.close();

	cout << "Simulation details:" << endl;
	cout << "steps_equil = " << steps_equil << endl;
	cout << "hours_equil = " << hours_equil << endl;
	cout << "steps_prod = " << steps_prod << endl;
	cout << "hours_prod = " << hours_prod << endl;
	cout << "skip_steps = " << skip_steps << endl;
	cout << "skip_steps_traj = " << skip_steps_traj << endl;
	cout << "skip_steps_check = " << skip_steps_check << endl;
	cout << "dr_max = " << dr_max << endl;
	cout << "frac_trans = " << frac_trans << endl;
	cout << "frac_swap = " << frac_swap << endl;
	cout << "seed = " << seed << endl;
	cout << endl;
}

void RandomSequentialAddition(State &state, int seed){

        //random number generator
        mt19937_64 rng_x, rng_y; //, rng_z;
        rng_x.seed(12+seed); rng_y.seed(100+seed); //rng_z.seed(3+seed);
        uniform_real_distribution<double> unif(0, state.L);

        cout << "Starting RSA generation at eta = " << eta << endl;


        //loop over the types and how many of them
        Particle particle;
        bool overlap_free;
        int attempt = 0;
        int index = 0;
        for (int i = 0; i < N; i++){

                //loop over attempts to insert desired number of each type
                attempt = 0; n = 0;
                while (attempt < max_attempts && n < state.type_to_N_type[i]){
                        particle.rx = unif(rng_x); particle.ry = unif(rng_y); particle.rz = unif(rng_z);
                        particle.type = i;
                        particle.index = index;

                        //check for an overlap
                        overlap_free = ParCheckParticleOverlapRSA(state, particle);

                        //add new particle if possible
                        if (overlap_free){
                                attempt = 0; n++; state.particles.push_back(particle);
                                index++;
                        }
                        else{
                                attempt++;
                        }
                }
        }

        //provide some feedback
        if (attempt < max_attempts)
                cout << "RSA completed!" << endl << endl;

}

//function definitions
void LoadSimulationData(
	long long int &steps_equil, double &hours_equil,
	long long int &steps_prod, double &hours_prod,
	int &skip_steps, int &skip_steps_traj, long long int &skip_steps_check, double &dr_max, double &frac_trans, double &frac_swap, int &seed);

int main(){
	//the simulation state
	State state;
	State state_new;

	//load in the relevant simulation details
	long long int steps_equil, steps_prod, skip_steps_check;
	double hours_equil, hours_prod;
	int skip_steps, skip_steps_traj, seed;
	double dr_max;
	bool read_state;
	LoadSimulationData(steps_equil, hours_equil, steps_prod, hours_prod, skip_steps, skip_steps_traj, skip_steps_check, dr_max, seed);

        //read in an initial state or perform random sequential addition
        read_state = ReadState("state_init.txt", state);
        if (!read_state){
                RandomSequentialAddition(state, seed);
                WriteState("state_rsa.txt", state);
        }
        state.Initialize();

}

