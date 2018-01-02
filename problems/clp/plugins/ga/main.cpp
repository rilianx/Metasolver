/*
 * main_clp.cpp
 *
 *  Created on: 29 jun. 2017
 *      Author: iaraya
 */

#include <iostream>
//#include "objects/State.cpp"
#include "clpState.h"
#include "clpStatekd.h"
#include "BlockSet.h"
#include "VCS_Function.h"
#include "SpaceSet.h"
#include "DoubleEffort.h"
#include "GlobalVariables.h"
#include "NSGA2.h"
#include "clp_chromosome.h"
#include "args.hxx"

bool global::TRACE = false;

using namespace std;

// para ejecutar (menos de 30 tipos de caja): BSG_CLP problems/clp/benchs/BR/BR7.txt 1 1.0 30 4.0 1.0 0.2 0.04 1.0 0.0 0.0 0 0
// para ejecutar (mas de 30 tipos de caja): BSG_CLP problems/clp/benchs/BR/BR8.txt 1 0.98 30 4.0 1.0 0.2 0.04 1.0 0.0 0.0 0 0


int main(int argc, char** argv){

	args::ArgumentParser parser("********* NSGA-II CLP *********.", "NSGA-II Solver for CLP.");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::ValueFlag<int> _inst(parser, "int", "Instance", {'i'});
	args::ValueFlag<double> _min_fr(parser, "double", "Minimum volume occupied by a block (proportion)", {"min_fr"});
	args::ValueFlag<int> _maxiter(parser, "int", "Number of iteration of the NSGA-II", {"maxiter"});
	args::ValueFlag<int> _popsize(parser, "int", "Population of the NSGA-II", {"pop"});
	args::ValueFlag<double> _pmut(parser, "int", "Probability of mutation (NSGA-II)", {"pmut"});
	args::ValueFlag<double> _pcr(parser, "int", "Probability of crossover (NSGA-II)", {"pcr"});
	args::ValueFlag<double> _maxrank(parser, "int", "Max ranking of genes (NSGA-II)", {"maxrank"});
	args::ValueFlag<int> _seed(parser, "int", "Random seed", {"seed"});
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
	int maxiter=(_maxiter)? _maxiter.Get():100;
	int popsize=(_popsize)? _popsize.Get():50;
	double pmut=(_pmut)? _pmut.Get():0.1;
	double pcr=(_pcr)? _pcr.Get():0.4;
	GenCLP::max_rank=(_maxrank)? _maxrank.Get():100;
	int seed=(_seed)? _seed.Get():1;
	srand(seed);


    cout << "cargando la instancia..." << endl;

    Block::FSB=fsb;
    clpState* s0 = new_state(file,inst, min_fr, 10000);

    cout << "n_blocks:"<< s0->get_n_valid_blocks() << endl;
    clock_t begin_time=clock();

    VCS_Function* vcs = new VCS_Function(s0->nb_left_boxes, *s0->cont);

    NSGA2* nsga2= new NSGA2(new ChromosomeFactoryCLP(), vcs, pmut, pcr, popsize, maxiter, trace);

	//cout << "double effort" << endl;
    //SearchStrategy *de= new DoubleEffort(*bsg);

	cout << "copying state" << endl;
	State& s_copy= *s0->clone();

   // cout << s0.valid_blocks.size() << endl;

	cout << "running" << endl;
    double eval = 1-nsga2->run(s_copy) ;
	cout << eval << endl;



	//dynamic_cast<const clpState*>(nsga2->get_best_state())->cont->MatLab_print();


}
