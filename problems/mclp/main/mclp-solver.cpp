/*
 * main_clp.cpp
 *
 *  Created on: 29 jun. 2017
 *      Author: iaraya
 */
#include <iostream>
#include <fstream>
#include <unistd.h>

#include <dirent.h>
#include <malloc.h>
#include <iterator>

#include "args.hxx"
//#include "objects/State.cpp"
#include "mclp-state.h"
#include "BlockSet.h"
#include "BoxShape.h"
#include "Greedy.h"
#include "DoubleEffort.h"
#include "GlobalVariables.h"
#include "BSG.h"
#include "../methods/MCLPSolver.h"

bool global::TRACE = false;


using namespace std;
using namespace mclp;



void dfsPrintChild(const State* node, ofstream& file){
	file << "{ "<<endl;
	file<<"\t \"name\":\""<<node->get_id()<<"\",";
	file<<"\t \"parent\":\""<<node->get_parent()->get_id() <<"\",";
	file<<"\t \"value\":\""<<node->get_value() <<"\",";
	file<<"\t \"sd\":\"\",";
	file<<"\t \"mcts_value\":\"\",";
	file<<"\t \"stimated_sd\":\"\",";
	file<<"\t \"ponderated_sd\":\"\",";
	file<<"\t \"depth\":\"\",";
	file<<"\t \"num_visit\":\"\"";
	file<< "\t,\"simulations\":[]"<<endl;


	if(!node->get_children().empty()){
		file<< "\t,\"children\":["<<endl;
		for(auto c:node->get_children()){
			dfsPrintChild(c,file);
			if(c!=node->get_children().back())
			    file<<","<<endl;
		}
		file<<"]";
	}
	file<<"}"<<endl;
}

void pointsToTxt(State* root, int it) {
	ofstream scp("problems/clp/tree_plot/flare"+std::to_string(it)+".json");
	dfsPrintChild(root,scp);
}
int main(int argc, char** argv){

	args::ArgumentParser parser("********* BSG-CLP *********.", "BSG Solver for CLP.");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::ValueFlag<int> _inst(parser, "int", "Instance", {'i'});
	args::Flag _rotate(parser, "rotate", "rotations are allowed?", {"rot"});
	args::ValueFlag<int> _nboxes(parser, "int", "Number of boxes for each type", {"nboxes"});
	args::ValueFlag<int> _nbins(parser, "int", "Number of bins to be generated", {"nbins"});
	args::ValueFlag<double> _pdec(parser, "int", "Priority decreasing ratio", {"pdec"});
	args::ValueFlag<double> _min_fr(parser, "double", "Minimum volume occupied by a block (proportion)", {"min_fr"});
	args::ValueFlag<double> _first_val(parser, "double", "first restriction for pow at profit", {"first_val"});
	args::ValueFlag<double> _second_val(parser, "double", "second restriction for pow at profit", {"second_val"});

	args::ValueFlag<int> _maxtime(parser, "int", "Timelimit", {'t', "timelimit"});
	args::ValueFlag<int> _seed(parser, "int", "Random seed", {"seed"});
	args::ValueFlag<double> _break_value(parser, "double", "Break Bins Porcent", {"break_value"});
  args::ValueFlag<double> _prob(parser, "double", "Technic to use to break bins", {"prob_tec"});

	args::ValueFlag<double> _solver_iter(parser, "int", "number of iterations by the solver", {"solver_iter"});
	args::ValueFlag<string> _gurobi_path(parser, "string", "path of gurobi", {"gurobi_path"});

	args::Flag _plot(parser, "double", "plot tree", {"plot"});
	//args::Flag fsb(parser, "fsb", "full-support blocks", {"fsb"});
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

	string gurobi_path = "extras/gurobi";
	string file=_file.Get();
	int inst=(_inst)? _inst.Get():0;
	double min_fr=(_min_fr)? _min_fr.Get():0.98;
	double break_value = 0.5;
	int maxtime=(_maxtime)? _maxtime.Get():100;
	int solver_iter = 1;
  double prob = 0.5;
	double first_val = 0.8;
	double second_val = 1.0;

	double alpha=3.0, beta=2.0, gamma=0.5, delta=1.0, p=0.04, pdec=0.8;
	int nbins=1000;
	int nboxes=1;
	if(_maxtime) maxtime=_maxtime.Get();
	if(_nboxes) nboxes=_nboxes.Get();
	if(_pdec) pdec=_pdec.Get();
	if(_nbins) nbins=_nbins.Get();
	if(_break_value) break_value=_break_value.Get();
  if(_gurobi_path) gurobi_path = _gurobi_path.Get();
	if(_solver_iter)	solver_iter = _solver_iter.Get();
  if(_prob)	prob = _prob.Get();
	if(_first_val) first_val = _first_val.Get();
	if(_second_val) second_val = _second_val.Get();

	int seed=(_seed)? _seed.Get():1;
	srand(seed);
	pair<double,double> limits;
	limits.first = first_val;
	limits.second = second_val;

	global::TRACE = trace;

 cout << "cargando la instancia..." << endl;

//a las cajas se les inicializan sus pesos en 1

	cout << "***** Creando el contenedor ****" << endl;
	cout << "Instance:" << inst+1 << endl;
	cout << "min_fr:" << min_fr << endl;
	cout << "Maxtime:" << maxtime << endl;
	cout << "Rotations allowed:" << _rotate << endl;
	cout << "N_bins:" << nbins << endl;
	cout << "Decreasing ratio (priority):" << pdec << endl;

  clock_t begin_time=clock();
	mclpState* s0 = new_mstate(file,inst, min_fr, 10000, _rotate, nboxes);

    cout << "n_blocks:"<< s0->get_n_valid_blocks() << endl;

    VCS_Function* vcs = new VCS_Function(s0->nb_left_boxes, *s0->cont,
        alpha, beta, gamma, p, delta, 0.0, 0.0);

	cout << "greedy" << endl;
	Greedy *gr = new Greedy (vcs);


	cout << "bsg" << endl;
	BSG *bsg= new BSG(vcs,*gr, 4, 0.0, 0, _plot);
	bsg->trace=false;

	MCLPSolver *solver;
	solver = new MCLPSolver(gurobi_path,solver_iter,break_value,nbins,s0);
	int bins = solver->solver(gr, pdec, prob, limits);
	//int bins = solver->solve(bsg, s0, pdec, prob, limits);

  //int bins=solve(gr, bsg, s0, nbins, pdec, gurobi_path, solver_iter, break_value);
  //std::cout << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;
	//if(_plot){
	//   pointsToTxt(&s_copy, 0);
	//   system("firefox problems/clp/tree_plot/index.html");
	//}
	cout << bins << " " << solver->getlastUpdate()
			/*<< " " <<float( clock () - begin_time ) /  CLOCKS_PER_SEC*/ <<endl;
}
