/*
 * main_clp.cpp
 *
 *  Created on: 29 jun. 2017
 *      Author: iaraya
 */

#include <iostream>
#include "args.hxx"
#include "clpState.h"
#include "VCS_Function.h"
#include "Greedy.h"
#include "DoubleEffort.h"
#include "BSG.h"
#include "LNS.h"

using namespace std;
using namespace clp;

// ejemplo: BSG_CLP problems/clp/benchs/BR/BR7.txt -i 0 -t 30

int main(int argc, char** argv){

	args::ArgumentParser parser("********* BSG-CLP *********.", "BSG Solver for CLP.");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::ValueFlag<int> _inst(parser, "int", "Instance", {'i'});
	args::ValueFlag<double> _min_fr(parser, "double", "Minimum volume occupied by a block (proportion)", {"min_fr"});
	args::ValueFlag<int> _maxtime(parser, "int", "Timelimit", {'t', "timelimit"});
	args::ValueFlag<double> _alpha(parser, "double", "Alpha parameter", {"alpha"});
	args::ValueFlag<double> _beta(parser, "double", "Beta parameter", {"beta"});
	args::ValueFlag<double> _gamma(parser, "double", "Gamma parameter", {"gamma"});
	args::ValueFlag<double> _delta(parser, "double", "Delta parameter", {"delta"});
	args::ValueFlag<double> _p(parser, "double", "p parameter", {'p'});
	args::Flag _fix_knapsack(parser, "fix_knapsack", "Volume loss: consider the lengths of all the allowed orientations of the boxes", {"fix_knapsack"});
	args::Flag _lns(parser, "lns", "Improve the best BSG solution with LNS (destroying central regions)", {"lns"});
	args::ValueFlag<string> _lns_shape(parser, "string", "LNS: shape of the destroyed region (box, slab). Default: box", {"lns_shape"});
	args::ValueFlag<double> _lns_frac(parser, "double", "LNS: proportion of the timelimit used by LNS. Default: 0.3", {"lns_frac"});
	args::ValueFlag<int> _lns_beams(parser, "int", "LNS: number of beams of the BSG used for repairing. Default: 4", {"lns_beams"});
	args::Positional<std::string> _file(parser, "instance-set", "The name of the instance set (BR format)");

	cout.precision(8);
	try{
		parser.ParseCLI(argc, argv);
	}catch (args::Help&){
		std::cout << parser;
		return 0;
	}catch (args::ParseError& e){
		std::cerr << e.what() << std::endl;
		std::cerr << parser;
		return 1;
	}catch (args::ValidationError& e){
		std::cerr << e.what() << std::endl;
		std::cerr << parser;
		return 1;
	}

	string file=_file.Get();
	int inst=(_inst)? _inst.Get():0;
	double min_fr=(_min_fr)? _min_fr.Get():0.98;
	int maxtime=(_maxtime)? _maxtime.Get():100;

	double alpha=4.0, beta=1.0, gamma=0.2, delta=1.0, p=0.04;
	if(_alpha) alpha=_alpha.Get();
	if(_beta) beta=_beta.Get();
	if(_gamma) gamma=_gamma.Get();
	if(_delta) delta=_delta.Get();
	if(_p) p=_p.Get();

	cout << "***** Creando el contenedor ****" << endl;
	cout << "File: " << file << endl;
	cout << "Instance:" << inst+1 << endl;
	cout << "min_fr:" << min_fr << endl;
	cout << "Maxtime:" << maxtime << endl;

	//el tiempo incluye la lectura de la instancia y la generacion de bloques
	clock_t begin_time=clock();

	clpState* s0 = new_state(file, inst, min_fr, 10000);

	cout << "n_blocks:"<< s0->get_n_valid_blocks() << endl;

	VCS_Function* vcs = new VCS_Function(clpState::types, s0->left, s0->valid_blocks,
			s0->cont.getL(), s0->cont.getW(), s0->cont.getH(), alpha, beta, gamma, p, delta, _fix_knapsack);

	SearchStrategy *gr = new Greedy(vcs);
	BSG *bsg= new BSG(vcs, *gr, 4);
	SearchStrategy *de= new DoubleEffort(*bsg);

	State& s_copy= *s0->clone();

	double lns_frac = (_lns)? ((_lns_frac)? _lns_frac.Get() : 0.3) : 0.0;

	double eval=de->run(s_copy, maxtime*(1.0-lns_frac), begin_time);

	if(_lns){
		clpLNS::Shape shape = clpLNS::BOX;
		if(_lns_shape){
			if(_lns_shape.Get()=="slab") shape = clpLNS::SLAB;
			else if(_lns_shape.Get()!="box") { cerr << "Unknown LNS shape: " << _lns_shape.Get() << endl; return 1; }
		}
		int lns_beams = (_lns_beams)? _lns_beams.Get() : 4;

		clpLNS lns(*s0, vcs, *gr, shape, lns_beams);
		clpState* best = lns.run(*static_cast<const clpState*>(de->get_best_state()), maxtime, begin_time);
		eval = best->get_value();
		if(!best->is_feasible()) { cerr << "ERROR: infeasible LNS solution" << endl; return 1; }
		cout << "LNS iterations: " << lns.get_iterations() << endl;
	}

	if(!static_cast<const clpState*>(de->get_best_state())->is_feasible()){
		cerr << "ERROR: infeasible solution" << endl;
		return 1;
	}

	cout << "% volume utilization" << endl;
	cout << eval*100 << endl;
}
