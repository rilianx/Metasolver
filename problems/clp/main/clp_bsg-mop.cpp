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
#include "Greedy.h"
#include "DoubleEffort.h"
#include "GlobalVariables.h"
#include "BSGMOP.h"
#include "args.hxx"

bool global::TRACE = false;

using namespace std;


int main(int argc, char** argv){

	args::ArgumentParser parser("********* BSG-B *********.", "BSG-B Solver for CLP.");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::ValueFlag<int> _inst(parser, "int", "Instance number", {'i'});
	args::ValueFlag<string> _format(parser, "string", "Format (BR, BRw, 1C, BRwp)", {'f'});
	args::ValueFlag<string> _formatp(parser, "string", "Kind of profit (R, I, W)", {"fp"});
  args::ValueFlag<double> _min_fr(parser, "double", "Block_generation: Minimum volume occupied by a block (proportion)", {"min_fr"});
	args::ValueFlag<int> _maxblocks(parser, "int", "Block_generation:  Maximum number ob generated blocks", {"maxb"});
	args::ValueFlag<int> _maxtime(parser, "int", "Timelimit", {'t', "timelimit"});
	args::ValueFlag<int> _seed(parser, "int", "Random seed", {"seed"});
	args::ValueFlag<string> _strategy(parser, "double", "Strategy (greedy, bsg, bsg_p, bsg_vp, ns+cd, o-search)", {'s'});
  args::ValueFlag<string> _theta_v(parser, "double", "VPD's parameter vector (max vol)", {"theta_v"});
  args::ValueFlag<string> _theta_p(parser, "double", "VPD's parameter vector (max profit)", {"theta_p"});
    args::ValueFlag<int> _nsample(parser, "int", "Maximum number of actions to be evaluated by greedy (default: 0 (max))", {"nsample"});
	args::ValueFlag<int> _write_tree_search(parser, "double", "Write a tree search steps (nb_of_beams)", {"wts"});

  args::ValueFlag<string> _ref(parser, "\"double double\"", "Reference point. (format: \"y1 y2\")", {"ref"});

  args::Flag _print(parser, "double", "Print output", {"print"});

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
		BSG_MOP::sel_rule srule=BSG_MOP::NSGA2;
		int inst=(_inst)? _inst.Get():0;
		double min_fr=(_min_fr)? _min_fr.Get():1.0;
		int maxtime=(_maxtime)? _maxtime.Get():100;
		int max_blocks=(_maxblocks)? _maxblocks.Get():10000;
		string strategy=(_strategy)? _strategy.Get():"o-search";
		if(_maxtime) maxtime=_maxtime.Get();


		if(strategy=="o-search" || strategy=="ns+cd")
			srule = BSG_MOP::NSGA2;
		else if(strategy=="bsg" || strategy=="greedy")
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

	int nsample=0;
	if (_nsample) nsample=_nsample.Get();
    SearchStrategy *gr = new Greedy (vcs, nsample);

    int beams=4;
    if(_write_tree_search)
		   beams=_write_tree_search.Get();

    BSG_MOP *bsg= new BSG_MOP(vcs,*gr, beams, 0.0, 0, (strategy=="o-search"), srule );
		bsg->generate_tree_search_output=_write_tree_search;

    SearchStrategy *de= new DoubleEffort(*bsg);

		State& s_copy= *s0->clone();

   //gr->run(s_copy, maxtime, begin_time) ;

   //cout << 1-s_copy.get_value() << " " << 1-s_copy.get_value2() << endl;
	 //return 0;

	cout << "***** Running the solver BSGMOP solver *****" << endl;
	double eval;
	if(!_write_tree_search){
		if(strategy=="greedy"){
			eval=1-gr->run(s_copy, maxtime, begin_time) ;
			cout << "best_solution:" << endl;
			cout << s_copy.get_value() << endl;
			cout << *dynamic_cast<const clpState*>(&s_copy)->cont << endl;
			return 0;
		}else
		 	eval=1-de->run(s_copy, maxtime, begin_time) ;
	}else
		eval=1-bsg->run(s_copy, maxtime, begin_time) ;

    if(strategy=="bsg_vp"){
    	cout << "running with bsg_p" << endl;
    	s_copy= *s0->clone();
    	bsg->set_rule(BSG_MOP::MIN2);
      begin_time=clock();
      vcs->set_parameters (theta_p);
      bsg->set_beams(4); bsg->initialize();
      eval = 1-de->run(s_copy, maxtime, begin_time) ;
    }

    cout << "pareto_front" << endl;
    map< pair<double, double>, State*> pareto = bsg->get_pareto_front();
    double x_old=ref.first;
    double hv = 0.0;
    int n=0;
		double best_volume=0.0, best_profit=0.0, nadir_volume=0.0, nadir_profit=0.0;

    for(auto point : pareto){
			//the point does not strict dominate ref

		
    	if(best_profit==0.0 && point.second) {
           best_profit=point.first.second;
           nadir_volume=point.first.first;
       }

    	if(point.second){
    		n++;
    		best_volume=point.first.first;
        nadir_profit=point.first.second;
    	}

      if(ref.first >= point.first.first || ref.second >= point.first.second ) continue;

    	hv += (point.first.first - x_old) * (point.first.second-ref.second);
        //cout << x_old << " " << point.first.second << " - " ;
    	x_old = point.first.first;
    	cout << point.first.first << " " << point.first.second << endl;
    }
    cout << endl;
		cout.precision(4);
    cout << "Vmax\tPmax\tHV\t#sols" << endl;
    cout << best_volume <<  " " << best_profit << " " << hv <<  " " <<  n << endl;

    if(_print){
		cout << "best_solution:" << endl;
    	map< pair<double, double>, State*> ::iterator it = pareto.end();
    	it--;
    	it--;
		cout << best_volume << endl;
    	cout << *dynamic_cast<const clpState*>(it->second)->cont;
    }


	//dynamic_cast<const clpState*>(de->get_best_state())->cont->MatLab_print();


}
