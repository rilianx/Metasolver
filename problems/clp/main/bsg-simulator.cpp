/*
 * main_clp.cpp
 *
 *  Created on: 29 jun. 2017
 *      Author: iaraya
 */

#include <iostream>
#include <vector>
#include "clpState.h"
#include "VCS_Function.h"
#include "SpaceSet.h"
#include "Simulation.h"
#include "DoubleEffort.h"
#include "GlobalVariables.h"
#include "BSGMOP.h"
#include "args.hxx"

bool global::TRACE = false;

using namespace std;


int main(int argc, char** argv){

	args::ArgumentParser parser("********* BSG-SIMULATOR *********.", "BSG-B Solver for CLP.");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::ValueFlag<int> _inst(parser, "int", "Instance number", {'i'});
	args::ValueFlag<string> _actions(parser, "list", "Actions separated by comma", {"actions"});
	args::ValueFlag<string> _format(parser, "string", "Format (BR, BRw, 1C, BRwp)", {'f'});
	args::ValueFlag<string> _formatp(parser, "string", "Kind of profit (R, I, W)", {"fp"});
  args::ValueFlag<double> _min_fr(parser, "double", "Block_generation: Minimum volume occupied by a block (proportion)", {"min_fr"});
	args::ValueFlag<int> _maxblocks(parser, "int", "Block_generation:  Maximum number ob generated blocks", {"maxb"});
	args::ValueFlag<int> _maxtime(parser, "int", "Timelimit", {'t', "timelimit"});
	args::ValueFlag<int> _seed(parser, "int", "Random seed", {"seed"});
	args::ValueFlag<string> _strategy(parser, "double", "Strategy (bsg, bsg_p, bsg_vp, ns+cd, o-search)", {'s'});
    args::ValueFlag<string> _theta_v(parser, "double", "VPD's parameter vector (max vol)", {"theta_v"});
    args::ValueFlag<string> _theta_p(parser, "double", "VPD's parameter vector (max profit)", {"theta_p"});

  args::ValueFlag<string> _ref(parser, "\"double double\"", "Reference point. (format: \"y1 y2\")", {"ref"});

  args::Flag _matlab(parser, "double", "Matlab output", {"matlab"});

	args::Flag fsb(parser, "fsb", "full-support blocks", {"fsb"});
	args::Flag trace(parser, "trace", "Trace", {"trace"});
	args::Positional<std::string> _file(parser, "instance-set", "The instance set");

		cout.precision(8);
		try
		{
			parser.ParseCLI(argc, argv);

		}
		catch (args::Help&)
		{
			std::cout << parser;
			return 0;
		}
		catch (args::ParseError& e)
		{
			std::cerr << e.what() << std::endl;
			std::cerr << parser;
			return 1;
		}
		catch (args::ValidationError& e)
		{
			std::cerr << e.what() << std::endl;
			std::cerr << parser;
			return 1;
		}

		string file=_file.Get();
		string actions_str = _actions.Get();
		BSG_MOP::sel_rule srule=BSG_MOP::MIN1;
		int inst=(_inst)? _inst.Get():0;
		double min_fr=(_min_fr)? _min_fr.Get():1.0;
		int maxtime=(_maxtime)? _maxtime.Get():100;
		int max_blocks=(_maxblocks)? _maxblocks.Get():10000;
		string strategy=(_strategy)? _strategy.Get():"bsg";
		if(_maxtime) maxtime=_maxtime.Get();


		if(strategy=="o-search" || strategy=="ns+cd")
			srule = BSG_MOP::NSGA2;
		else if(strategy=="bsg")
			srule = BSG_MOP::MIN1;
		else if(strategy=="bsg_p")
			srule = BSG_MOP::MIN2;
		else if(strategy=="bsg_vp"){
			srule = BSG_MOP::MIN1;
			maxtime /=2;
		}

		string format="BR";
		if(_format) format=_format.Get();

		string formatp="in_file";
		if(_formatp) formatp=_formatp.Get();

		clpState::Format f;
		clpState::FormatP fp;
		if(format=="BR")
			f=clpState::BR;
		else if(format=="BRw")
			f=clpState::BRw;
		else if(format=="1C")
			f=clpState::_1C;
		else if(format=="BRwp")
			f=clpState::BRwp;

		if(formatp=="in_file" || formatp=="R")
			fp=clpState::NORMAL;
		else if(formatp=="all_one" || formatp=="I")
			fp=clpState::ALL_ONE;
		else if(formatp=="weight" || formatp=="W")
			fp=clpState::WEIGHT;

    pair<double,double> ref = make_pair(0.0,0.0);
    if(_ref){
			std::string s = _ref.Get();
			std::istringstream is( s );
			is >> ref.first;
			is >> ref.second;
		}

		int seed=(_seed)? _seed.Get():1;
		srand(seed);

		global::TRACE = trace;

   // cout << "cargando la instancia..." << endl;

    //a las cajas se les inicializan sus pesos en 1

	cout << "***** Creando el contenedor ****" << endl;
	cout << "File("<< format <<"): " << file << endl;
	cout << "Instance:" << inst+1 << endl;
	cout << "min_fr:" << min_fr << endl;
	cout << "max_blocks:" << max_blocks << endl;
	cout << "Maxtime:" << maxtime << endl;
	cout << "ref_point: (" << ref.first << "," << ref.second << ")" << endl;

	clock_t begin_time=clock();

	Block::FSB=fsb;
    clpState* s0 = new_state(file,inst, min_fr, max_blocks, f, fp);
	cout << "Dimensions: " << s0->cont->getL() << " x " << s0->cont->getW() << " x " << s0->cont->getH() << endl;
  cout << "Number of generated blocks: "<< s0->get_n_valid_blocks() << endl;
	cout << "Strategy: " << strategy << endl ;

  //default values obtained with i-race and BRwp instances
  vector<double> theta_v{1.0,0.0,-0.13,0.02,3.85,6.27,0.48};
	vector<double> theta_p{-0.95,1.0,-0.16,0.02,1.53,1.53,0.44};

    if(_theta_v){
			string s= _theta_v.Get();
			char cstr[s.size() + 1];
			strcpy(cstr, s.c_str());

			char* pEnd=cstr;
			for(int i=0;i<theta_v.size();i++)
				theta_v[i] = strtod (pEnd, &pEnd);
		}

		if(_theta_p){
			string s= _theta_p.Get();
			char cstr[s.size() + 1];
			strcpy(cstr, s.c_str());

			char* pEnd=cstr;
			for(int i=0;i<theta_p.size();i++)
				theta_p[i] = strtod (pEnd, &pEnd);
		}

    cout << endl << "***** Creating the heuristic function VPD *****" << endl;
		cout << "theta_v: ";
		for (int i = 0; i < theta_v.size(); i++) {
			std::cout << theta_v.at(i) << ' ';
		}
		cout << endl;

		cout << "theta_p: ";
		for (int i = 0; i < theta_p.size(); i++) {
			std::cout << theta_p.at(i) << ' ';
		}
		cout << endl << endl;


	vector<double> theta=theta_v;
	if(strategy=="bsg_p") theta=theta_p;
	VCS_Function* vcs = new VCS_Function(s0->nb_left_boxes, *s0->cont, theta);
    vcs->set_parameters2 (theta_p);

    std::replace(actions_str.begin(), actions_str.end(), '(', ' ');  // replace ':' by ' '
    std::replace(actions_str.begin(), actions_str.end(), ')', ' ');  // replace ':' by ' '

    std::replace(actions_str.begin(), actions_str.end(), ';', ' ');  // replace ':' by ' '

    vector< list<int> > action_lists;
    stringstream ss(actions_str);
    string temp;
    while (ss >> temp){
    	std::replace(temp.begin(), temp.end(), ',', ' ');  // replace ':' by ' '
    	stringstream ss2(temp);
    	int temp2;
    	list<int> actions;
    	while (ss2 >> temp2)
    		actions.push_back(temp2); // done! now array={102,330,3133,76531,451,000,12,44412}
    	action_lists.push_back(actions);
    }


    Simulation *sim = new Simulation (vcs);
    cout << endl;
    int i=1;
    for(auto actions:action_lists){
    	cout << endl << "##start simulation " << i << endl;
    	sim->set_actions(actions);
  	    State& s_copy= *s0->clone();
        double eval = sim->run(s_copy, maxtime, begin_time) ;
        cout << eval << endl;
        cout << "##end simulation " << i << endl;
        i++;
    }



}
