/*
 * main_clp.cpp
 *
 *  Created on: 29 jun. 2017
 *      Author: iaraya
 */

#include <iostream>
#include "args.hxx"
//#include "objects/State.cpp"
#include "clpState.h"
#include "clpStatekd.h"
#include "BlockSet.h"
#include "MCTS.h"
#include "VCS_Function.h"
#include "VCS_Function.h"
#include "SpaceSet.h"
#include "Greedy.h"
#include "DoubleEffort.h"
#include "GlobalVariables.h"

bool global::TRACE = false;

using namespace std;



int main(int argc, char** argv){


	args::ArgumentParser parser("********* MCTS-CLP *********.", "MCTS Solver for CLP.");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::ValueFlag<int> _inst(parser, "int", "Instance", {'i'});
	args::ValueFlag<string> _format(parser, "string", "Format: (BR, BRw, 1C)", {'f'});
	args::ValueFlag<double> _min_fr(parser, "double", "Minimum volume occupied by a block (proportion)", {"min_fr"});
	args::ValueFlag<int> _maxtime(parser, "int", "Timelimit", {'t', "timelimit"});
	args::ValueFlag<int> _seed(parser, "int", "Random seed", {"seed"});
	args::ValueFlag<double> _alpha(parser, "double", "Alpha parameter", {"alpha"});
	args::ValueFlag<double> _beta(parser, "double", "Beta parameter", {"beta"});
	args::ValueFlag<double> _gamma(parser, "double", "Gamma parameter", {"gamma"});
	args::ValueFlag<double> _delta(parser, "double", "Delta parameter", {"delta"});
	args::ValueFlag<double> _p(parser, "double", "p parameter", {'p'});
	args::ValueFlag<double> _r(parser, "double", "random factor", {'r'});


	args::Flag fsb(parser, "fsb", "full-support blocks", {"fsb"});

	args::ValueFlag<double> _eps(parser, "double", "epsilon parameter for MCTS", {"eps"});
	args::Flag bp(parser, "bp", "backpropagation in MCTS", {"bp"});
	args::ValueFlag<int> _max_nodes(parser, "int", "maximum number of nodes in the MCTS", {"max_nodes"});
	args::Flag discard_equivalent_nodes(parser, "bp", "discard equivalent nodes in MCTS", {"den"});

	args::Flag trace(parser, "trace", "Trace", {"trace"});
	args::Positional<std::string> _file(parser, "instance-set", "The name of the instance set");


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
	int inst=(_inst)? _inst.Get():0;
	double min_fr=(_min_fr)? _min_fr.Get():0.98;
	int maxtime=(_maxtime)? _maxtime.Get():100;

	double alpha=4.0, beta=1.0, gamma=0.2, delta=1.0, p=0.04, maxtheta=0.0, eps=0.0001, r=0.0, max_nodes=100;
	if(_maxtime) maxtime=_maxtime.Get();
	if(_alpha) alpha=_alpha.Get();
	if(_beta) beta=_beta.Get();
	if(_gamma) gamma=_gamma.Get();
	if(_delta) delta=_delta.Get();
	if(_p) p=_p.Get();
	if(_r) r=_r.Get();
	if(_eps) eps=_eps.Get();
	if(_max_nodes) max_nodes=_max_nodes.Get();


	string format="BR";
	if(_format) format=_format.Get();

	clpState::Format f;
	if(format=="BR")
		f=clpState::BR;
	else if(format=="BRw")
		f=clpState::BRw;
	else if(format=="1C")
		f=clpState::_1C;

	int seed=(_seed)? _seed.Get():1;
	srand(seed);

	global::TRACE = trace;

	cout << "***** Creando el contenedor ****" << endl;
	cout << "File("<< format <<"): " << file << endl;
	cout << "Instance:" << inst+1 << endl;
	cout << "min_fr:" << min_fr << endl;
	cout << "Maxtime:" << maxtime << endl;



    Block::FSB=fsb;
    clpState* s0 = new_state(file,inst, min_fr, 10000, f);


    cout << "n_blocks:"<< s0->get_n_valid_blocks() << endl;

    clock_t begin_time=clock();

    VCS_Function* vcs = new VCS_Function(s0->nb_left_boxes, *s0->cont,
    alpha, beta, gamma, p, delta, 0.0, r);

	cout << "greedy" << endl;
    SearchStrategy *gr = new Greedy (vcs);

	cout << "bsg" << endl;
    MCTS *mcts= new MCTS(vcs,*gr,eps, max_nodes, bp, discard_equivalent_nodes);


	cout << "copying state" << endl;
	State& s_copy= *s0->clone();

   // cout << s0.valid_blocks.size() << endl;

	cout << "running" << endl;
    double eval = mcts->run(s_copy, maxtime, begin_time);

    cout << "best_volume  best_volume(weight) hypervolume" << endl;
	cout << eval << " " << mcts->get_best_state()->get_value2() << " " << eval*mcts->get_best_state()->get_value2() << endl;

   // cout << eval << endl;


/*
	list<const Action*>& actions= dynamic_cast<const clpState*>(de->get_best_state())->get_path();
    actions.sort(clpState::height_sort);


	clpState* s00 = dynamic_cast<clpState*> (s0->clone());

	for(auto action:actions){
		const clpAction* clp_action = dynamic_cast<const clpAction*> (action);
		s00->transition(*clp_action);
		//s00->cont
		//s00->nb_left_boxes;


		cout << "block :" << clp_action->block << endl;
		cout << "location :" << clp_action->space.get_location(clp_action->block) << endl;


	}
*/
/*
	const AABB* b = &dynamic_cast<const clpState*>(de->get_best_state())->cont->blocks->top();
	while(dynamic_cast<const clpState*>(de->get_best_state())->cont->blocks->has_next()){
		cout << *b << ":" << b->getVolume() << "(" << b->getOccupiedVolume() << ")" << endl;
		b = &dynamic_cast<const clpState*>(de->get_best_state())->cont->blocks->next();
	}
*/

	//s00->cont->MatLab_print();


}
