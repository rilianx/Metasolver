/*
 * main_clp.cpp
 *
 *  Created on: 29 jun. 2017
 *      Author: iaraya
 */

#include <iostream>
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

	args::ArgumentParser parser("********* BSG-MOP CLP *********.", "BSG-MOP Solver for CLP.");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::ValueFlag<int> _inst(parser, "int", "Instance", {'i'});
	args::ValueFlag<string> _format(parser, "string", "Format: (BR, BRw, 1C)", {'f'});
	args::ValueFlag<double> _min_fr(parser, "double", "Minimum volume occupied by a block (proportion)", {"min_fr"});
	args::ValueFlag<int> _maxtime(parser, "int", "Timelimit", {'t', "timelimit"});
	args::ValueFlag<int> _seed(parser, "int", "Random seed", {"seed"});
	args::ValueFlag<int> _alpha(parser, "double", "Alpha parameter", {"alpha"});
	args::ValueFlag<int> _beta(parser, "double", "Beta parameter", {"beta"});
	args::ValueFlag<int> _gamma(parser, "double", "Gamma parameter", {"gamma"});
	args::ValueFlag<int> _delta(parser, "double", "Delta parameter", {"delta"});
	args::ValueFlag<int> _p(parser, "double", "p parameter", {'p'});


	args::Flag fsb(parser, "fsb", "full-support blocks", {"fsb"});
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

		double alpha=4.0, beta=1.0, gamma=0.2, delta=1.0, p=0.04;
		if(_maxtime) maxtime=_maxtime.Get();
		if(_alpha) alpha=_alpha.Get();
		if(_beta) beta=_beta.Get();
		if(_gamma) gamma=_gamma.Get();
		if(_delta) delta=_delta.Get();
		if(_p) p=_p.Get();

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

   // cout << "cargando la instancia..." << endl;

    //a las cajas se les inicializan sus pesos en 1
    clpState* s0 = new_state(file,inst, min_fr, 10000, f);

    cout << "n_blocks:"<< s0->get_n_valid_blocks() << endl;

    clock_t begin_time=clock();


    VCS_Function* vcs = new VCS_Function(s0->nb_left_boxes, *s0->cont,
    alpha, beta, gamma, p, delta);

    SearchStrategy *gr = new Greedy (vcs);

    BSG_MOP *bsg= new BSG_MOP(vcs,*gr, 4);

    SearchStrategy *de= new DoubleEffort(*bsg);

	State& s_copy= *s0->clone();

    double eval = 1-de->run(s_copy, maxtime, begin_time) ;

    cout << "pareto_front" << endl;
    auto pareto = bsg->get_pareto_front();
    for(auto point : pareto){
    	cout << point.first.first << "," << point.first.second << endl;
    }
/*
 *
	const AABB* b = &dynamic_cast<const clpState*>(gr->get_best_state())->cont.blocks->top();
	while(dynamic_cast<const clpState*>(gr->get_best_state())->cont.blocks->has_next()){
		cout << *b << ":" << b->getVolume() << "(" << b->getOccupiedVolume() << ")" << endl;
		b = &dynamic_cast<const clpState*>(gr->get_best_state())->cont.blocks->next();
	}
*/

	//dynamic_cast<const clpState*>(de->get_best_state())->cont.MatLab_print();


}
