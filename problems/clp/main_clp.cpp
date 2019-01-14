/*
 * main_clp.cpp
 *
 *  Created on: 29 jun. 2017
 *      Author: iaraya
 */

#include <iostream>
#include <fstream>
#include "args.hxx"
//#include "objects/State.cpp"
#include "clpState.h"
#include "clpStatekd.h"
#include "BlockSet.h"
#include "BSG_midBSG.h"
#include "VCS_Function.h"
#include "VCS_Function.h"
#include "SpaceSet.h"
#include "Greedy.h"
#include "DoubleEffort.h"
#include "GlobalVariables.h"
#include "BSG.h"

bool global::TRACE = false;

using namespace std;

// para ejecutar (menos de 30 tipos de caja): BSG_CLP problems/clp/benchs/BR/BR7.txt 1 1.0 30 4.0 1.0 0.2 0.04 1.0 0.0 0.0 0 0
// para ejecutar (mas de 30 tipos de caja): BSG_CLP problems/clp/benchs/BR/BR8.txt 1 0.98 30 4.0 1.0 0.2 0.04 1.0 0.0 0.0 0 0




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
	ofstream myfile("problems/clp/tree_plot/flare"+std::to_string(it)+".json");
	dfsPrintChild(root,myfile);
}

int main(int argc, char** argv){


	args::ArgumentParser parser("********* BSG-CLP *********.", "BSG Solver for CLP.");
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
	args::Flag _plot(parser, "double", "plot tree", {"plot"});

	args::ValueFlag<double> _alphafactor(parser, "double", "Multiplication factor for alpha", {"alphafactor"});
	args::ValueFlag<double> _betafactor(parser, "double", "Multiplication factor for beta", {"betafactor"});
	args::ValueFlag<double> _gammafactor(parser, "double", "Multiplication factor for gamma", {"gammafactor"});
	args::ValueFlag<double> _deltafactor(parser, "double", "Multiplication factor for delta", {"deltafactor"});
	args::ValueFlag<double> _pfactor(parser, "double", "Multiplication factor for p", {"pfactor"});

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

	double alpha=4.0, beta=1.0, gamma=0.2, delta=1.0, p=0.04, maxtheta=0.0;
	if(_maxtime) maxtime=_maxtime.Get();
	if(_alpha) alpha=_alpha.Get();
	if(_beta) beta=_beta.Get();
	if(_gamma) gamma=_gamma.Get();
	if(_delta) delta=_delta.Get();
	if(_p) p=_p.Get();

	double alphafactor=1.0, betafactor=1.0, gammafactor=1.0, deltafactor=1.0, pfactor=1.0;
	if(_alphafactor) alphafactor=_alphafactor.Get();
	if(_betafactor) betafactor=_betafactor.Get();
	if(_gammafactor) gammafactor=_gammafactor.Get();
	if(_deltafactor) deltafactor=_deltafactor.Get();
	if(_pfactor) pfactor=_pfactor.Get();

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

// cout << "cargando la instancia..." << endl;

//a las cajas se les inicializan sus pesos en 1

	cout << "***** Creando el contenedor ****" << endl;
	cout << "File("<< format <<"): " << file << endl;
	cout << "Instance:" << inst+1 << endl;
	cout << "min_fr:" << min_fr << endl;
	cout << "Maxtime:" << maxtime << endl;

	double r=0.0; //0.0
    //bool kdtree= false;

    Block::FSB=fsb;
    clpState* s0 = new_state(file,inst, min_fr, 10000, f);

    //if(kdtree)
      // s0 = new clpState_kd(*s0);

    cout << "n_blocks:"<< s0->get_n_valid_blocks() << endl;

    clock_t begin_time=clock();

    VCS_Function* vcs = new VCS_Function(s0->nb_left_boxes, *s0->cont,
    alpha, beta, gamma, p, delta, 0.0, r, 1.5, alphafactor, betafactor, gammafactor, deltafactor, pfactor);

	/*if(kdtree){
		kd_block::set_vcs(*vcs);
		kd_block::set_alpha(alpha);
		kd_block::set_alpha(p);
	}*/

	//for(int i=0;i<10000; i++)
	//	exp->best_action(*s0);

	cout << "greedy" << endl;
    SearchStrategy *gr = new Greedy (vcs);

	cout << "bsg" << endl;
    BSG *bsg= new BSG(vcs,*gr, 4, 0.0, 0, _plot);
    //BSG_midBSG *bsg= new BSG_midBSG(*gr, *exp, 4);

    //bsg->set_shuffle_best_path(true);

	cout << "double effort" << endl;
    SearchStrategy *de= new DoubleEffort(*bsg);

	cout << "copying state" << endl;
	State& s_copy= *s0->clone();

   // cout << s0.valid_blocks.size() << endl;

	cout << "running" << endl;

    if(_plot)
    	de=bsg;

    double eval=de->run(s_copy, maxtime, begin_time) ;

    cout << "best_volume  best_volume(weight) hypervolume" << endl;
	cout << eval << " " << de->get_best_state()->get_value2() << " " << eval*de->get_best_state()->get_value2() << endl;
	
    cout << eval << endl;

    if(_plot){
    	pointsToTxt(&s_copy, 0);
    	system("firefox problems/clp/tree_plot/index.html");
    }

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
