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
#include "CPSAT.h"
#include <cstdlib>
#include <random>

using namespace std;
using namespace clp;

// ejemplo: BSG_CLP problems/clp/benchs/BR/BR7.txt -i 0 -t 30

int main(int argc, char** argv){

	args::ArgumentParser parser("********* BSG-CLP *********.", "BSG Solver for CLP.");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::ValueFlag<int> _inst(parser, "int", "Instance", {'i'});
	args::ValueFlag<double> _min_fr(parser, "double", "Minimum volume occupied by a block (proportion)", {"min_fr"});
	args::ValueFlag<int> _maxtime(parser, "int", "Timelimit", {'t', "timelimit"});
	args::ValueFlag<double> _alpha(parser, "double", "Alpha parameter. Default: 2.4407", {"alpha"});
	args::ValueFlag<double> _beta(parser, "double", "Beta parameter. Default: 2.2652", {"beta"});
	args::ValueFlag<double> _gamma(parser, "double", "Gamma parameter. Default: 0.3896", {"gamma"});
	args::ValueFlag<double> _delta(parser, "double", "Delta parameter", {"delta"});
	args::ValueFlag<double> _p(parser, "double", "p parameter. Default: 0.0164", {'p'});
	args::Flag _fix_knapsack(parser, "fix_knapsack", "Volume loss: consider the lengths of all the allowed orientations of the boxes", {"fix_knapsack"});
	args::Flag _portfolio(parser, "portfolio", "BSG: the root is expanded with greedy solutions using perturbed parameters (+-20% of alpha, beta, gamma, p); the best ones continue the search with their parameters", {"portfolio"});
	args::ValueFlag<double> _max_carried(parser, "double", "BSG: maximum proportion of the beam for paths carried from previous levels. Default: 1 (no limit)", {"max_carried"});
	args::Flag _lns(parser, "lns", "Improve the best BSG solution with LNS (destroying central regions)", {"lns"});
	args::ValueFlag<string> _lns_shape(parser, "string", "LNS: shape of the destroyed region (box, slab). Default: box", {"lns_shape"});
	args::ValueFlag<string> _lns_center(parser, "string", "LNS: center of the destroyed region (void, middle). Default: void", {"lns_center"});
	args::ValueFlag<double> _lns_frac(parser, "double", "LNS: proportion of the timelimit used by LNS. Default: 0.3", {"lns_frac"});
	args::ValueFlag<int> _lns_beams(parser, "int", "LNS: number of beams of the BSG used for repairing. Default: 4", {"lns_beams"});
	args::ValueFlag<int> _lns_samples(parser, "int", "LNS: destructions evaluated per iteration (the one with the largest potential improvement is repaired). Default: 8", {"lns_samples"});
	args::ValueFlag<string> _lns_repair(parser, "string", "LNS: repair (bsg, exact, hybrid). Default: bsg", {"lns_repair"});
	args::ValueFlag<string> _ortools(parser, "path", "LNS exact repair: OR-Tools library (e.g. .../site-packages/ortools/.libs/libortools.so.9). Default: $ORTOOLS_LIB", {"ortools_lib"});
	args::ValueFlag<double> _exact_time(parser, "double", "LNS exact repair: time limit per CP-SAT call. Default: 1.0", {"exact_time"});
	args::ValueFlag<int> _exact_boxes(parser, "int", "LNS exact repair: maximum number of candidate boxes. Default: 40", {"exact_boxes"});
	args::ValueFlag<int> _exact_redundant(parser, "0/1", "LNS exact repair: redundant constraints (cumulative per axis, volume). Default: 1", {"exact_redundant"});
	args::ValueFlag<int> _exact_workers(parser, "int", "LNS exact repair: CP-SAT workers. Default: 1", {"exact_workers"});
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

	//parametros ajustados por L. Guesser con irace (los de Araya et al. 2017 son
	//alpha=4, beta=1, gamma=0.2, p=0.04)
	double alpha=2.4407, beta=2.2652, gamma=0.3896, delta=1.0, p=0.0164;
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
	if(_max_carried) bsg->set_max_carried(_max_carried.Get());

	//portafolio de parametros: los parametros indicados, los de Araya et al. (2017) y
	//perturbaciones uniformes de +-20% de los parametros indicados
	std::mt19937 prng(1);
	int araya = vcs->add_params(VCS_Function::Params{4.0, 1.0, 0.2, 0.04, delta});
	if(_portfolio)
		bsg->set_root_sampler([&](const State& root, int k) -> State* {
			clpState* c = static_cast<clpState*>(root.clone());
			if(k==0) c->param = 0;
			else if(k==1) c->param = araya;
			else{
				std::uniform_real_distribution<double> u(0.8, 1.2);
				c->param = vcs->add_params(VCS_Function::Params{alpha*u(prng), beta*u(prng), gamma*u(prng), p*u(prng), delta});
			}
			return c;
		});
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
		clpLNS::Center center = clpLNS::VOID;
		if(_lns_center){
			if(_lns_center.Get()=="middle") center = clpLNS::MIDDLE;
			else if(_lns_center.Get()!="void") { cerr << "Unknown LNS center: " << _lns_center.Get() << endl; return 1; }
		}
		int lns_beams = (_lns_beams)? _lns_beams.Get() : 4;

		clpLNS::Repair repair = clpLNS::BSG_REPAIR;
		if(_lns_repair){
			if(_lns_repair.Get()=="exact") repair = clpLNS::EXACT_REPAIR;
			else if(_lns_repair.Get()=="hybrid") repair = clpLNS::HYBRID_REPAIR;
			else if(_lns_repair.Get()!="bsg") { cerr << "Unknown LNS repair: " << _lns_repair.Get() << endl; return 1; }
		}

		CpSolver* solver = NULL;
		clpLNS::ExactParams exact;
		if(repair!=clpLNS::BSG_REPAIR){
			string lib = (_ortools)? _ortools.Get() : (getenv("ORTOOLS_LIB")? getenv("ORTOOLS_LIB") : "");
			if(lib.empty()) { cerr << "The exact repair needs the OR-Tools library (--ortools_lib or $ORTOOLS_LIB)" << endl; return 1; }
			solver = new CpSolver(lib);
			if(_exact_time) exact.max_time = _exact_time.Get();
			if(_exact_boxes) exact.max_boxes = _exact_boxes.Get();
			if(_exact_workers) exact.workers = _exact_workers.Get();
			if(_exact_redundant) exact.redundant = _exact_redundant.Get()!=0;
		}

		int lns_samples = (_lns_samples)? _lns_samples.Get() : 8;

		clpLNS lns(*s0, vcs, *gr, shape, center, repair, solver, exact, lns_beams, lns_samples);
		clpState* best = lns.run(*static_cast<const clpState*>(de->get_best_state()), maxtime, begin_time);
		eval = best->get_value();
		if(!best->is_feasible()) { cerr << "ERROR: infeasible LNS solution" << endl; return 1; }
		cout << "LNS iterations: " << lns.get_iterations() << endl;
		if(solver) cout << "LNS improvements by exact repair: " << lns.get_exact_improvements() << endl;
	}

	if(!static_cast<const clpState*>(de->get_best_state())->is_feasible()){
		cerr << "ERROR: infeasible solution" << endl;
		return 1;
	}

	cout << "% volume utilization" << endl;
	cout << eval*100 << endl;
}
