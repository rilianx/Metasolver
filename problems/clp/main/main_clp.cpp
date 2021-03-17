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
#include "BlockSet.h"
#include "VCS_Function.h"
#include "SpaceSet.h"
#include "Greedy.h"
#include "DoubleEffort.h"
#include "GlobalVariables.h"
#include "BSG.h"

bool global::TRACE = false;

using namespace std;


int main(int argc, char** argv){


	args::ArgumentParser parser("********* BSG-CLP *********.", "BSG Solver for CLP.");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::ValueFlag<int> _inst(parser, "int", "Instance", {'i'});
	args::ValueFlag<string> _format(parser, "string", "Format: (BR, BRw, 1C, BRpc)", {'f'});
	args::ValueFlag<double> _min_fr(parser, "double", "Minimum volume occupied by a block (proportion)", {"min_fr"});
	args::ValueFlag<int> _maxtime(parser, "int", "Timelimit", {'t', "timelimit"});
	args::ValueFlag<int> _seed(parser, "int", "Random seed", {"seed"});
	args::ValueFlag<double> _alpha(parser, "double", "Alpha parameter", {"alpha"});
	args::ValueFlag<double> _beta(parser, "double", "Beta parameter", {"beta"});
	args::ValueFlag<double> _gamma(parser, "double", "Gamma parameter", {"gamma"});
	args::ValueFlag<double> _delta(parser, "double", "Delta parameter", {"delta"});
	args::ValueFlag<double> _p(parser, "double", "p parameter", {'p'});
	args::Flag _show_layout(parser, "layout", "Show the layout of the boxes in the best found solution", {"show_layout"});
	args::Flag _plot(parser, "double", "plot tree", {"plot"});


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


	string format="BR";
	if(_format) format=_format.Get();

	clpState::Format f;
	if(format=="BR")
		f=clpState::BR;
	else if(format=="BRw")
		f=clpState::BRw;
	else if(format=="BRpc")
		f=clpState::BRpc;
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

	double r=0.0;

    Block::FSB=fsb;
    //clpState* s0 = new_state(file,inst, min_fr, 10000, f);

	long L, W, H;
	double Wmax;

	//En este mapa se almacenan todas las cajas de la instancia junto a su cantidad
	map<BoxShape*, int> boxes;

	// se lee la instancia
	read_instance(L, W, H, Wmax, boxes, file, inst, f);

	//read_instanceMCLP(list<Volume*> TUs, list<double> Wmax, map<BoxShape*, int>, ...);

	//Algoritmo de particionamiento que selecciona cajas para los distintos TUs.

	// se genera un estado (un estado por TU)
	clpState* s0 = new_state(L, W, H, Wmax, boxes);


	//generación de bloques
	s0->general_block_generator(min_fr, 10000, *s0->cont);


	
	if(f==clpState::BRpc) s0->singlebox_blocks = new AABBList(); //for keeping boxes


    cout << "n_blocks:"<< s0->get_n_valid_blocks() << endl;

    clock_t begin_time=clock();

    VCS_Function* vcs = new VCS_Function(s0->nb_left_boxes, *s0->cont,
    alpha, beta, gamma, p, delta, 0.0, r);


	cout << "greedy" << endl;
    SearchStrategy *gr = new Greedy (vcs);

	cout << "bsg" << endl;
    BSG *bsg= new BSG(vcs,*gr, 8, 0.0, 0, _plot);

	//cout << "double effort" << endl;
    SearchStrategy *de= new DoubleEffort(*bsg);

	cout << "copying state" << endl;
	State& s_copy= *s0->clone();

	cout << "running" << endl;


    double eval=de->run(s_copy, maxtime, begin_time) ;

    cout << "% volume utilization" << endl;
	cout << eval*100.0 << endl;

	if(f==clpState::BRpc){
		if(dynamic_cast<const clpState*>(de->get_best_state())!=NULL){
			cout << "multidrop:" << dynamic_cast<const clpState*>(de->get_best_state())->multidrop() << endl;
			cout << "angle:" << dynamic_cast<const clpState*>(de->get_best_state())->loadbalanceA() << endl;
			cout << "completeShipment:" << dynamic_cast<const clpState*>(de->get_best_state())->completeshipment() << endl;
		}
	}
	




}
