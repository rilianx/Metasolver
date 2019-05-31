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
#include "VCS_Function.h"
#include "SpaceSet.h"
#include "DoubleEffort.h"
#include "GlobalVariables.h"
#include "BeamACO.h"
#include "Greedy.h"
#include "SearchStrategy.h"

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

/*----------------------------------------INICIACION DE VARIABLES-------------------------------------------*/

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

	args::ValueFlag<double> _alpha_media(parser, "double", "Weight of the priori knowledge", {"alpha_media"});
	args::ValueFlag<double> _aco_alpha(parser, "double", "Weight of the priori knowledge", {"aco_alpha"});
	args::ValueFlag<double> _aco_beta(parser, "double", "Weight of the priori knowledge", {"aco_beta"});
	args::ValueFlag<int> _metodo(parser, "int", "pheromone updating method (1 = update pheromone on only one state-action arc)(2 = update pheromone on the best found path)(3 = update pheromone on the best w paths, w being represented by the beams)", {"metodo"});
	args::ValueFlag<double> _mod_factor(parser, "double", "Value added to the pheromone factor in each iteration (recommend using values between 0.001 and 0.008)", {"mod_factor"});
	args::ValueFlag<double> _incremento(parser, "double", "Value added to tau", {"incremento"});
	args::ValueFlag<double> _std_dev(parser, "double", "Standard deviation of tau_m", {"std_dev"});


	args::Flag _plot(parser, "double", "plot tree", {"plot"});
	args::Flag _write_report(parser, "double", "...", {"write_report"});


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

	double alpha=4.0, beta=1.0, gamma=0.2, delta=1.0, p=0.04, maxtheta=0.0, aco_alpha=0.0, aco_beta=0.0, std_dev=0.0;
	if(_maxtime) maxtime=_maxtime.Get();
	if(_alpha) alpha=_alpha.Get();
	if(_beta) beta=_beta.Get();
	if(_aco_alpha) aco_alpha=_aco_alpha.Get();
	if(_aco_beta) aco_beta=_aco_beta.Get();
	if(_gamma) gamma=_gamma.Get();
	if(_delta) delta=_delta.Get();
	if(_p) p=_p.Get();
	if(_alpha_media) tau_matrix::alpha_media=_alpha_media.Get();
	if(_std_dev) std_dev=_std_dev.Get();


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

	static double incremento = 0.1;
	if(_incremento) incremento = _incremento.Get();
	BeamACO::aco_strategy metodo = BeamACO::ALL_PATHS;
	if(_metodo) metodo =(BeamACO::aco_strategy)_metodo.Get();
	static double mod_factor = 0.004;
	if(_mod_factor) mod_factor = _mod_factor.Get();

	/*cout << "Ingrese el metodo de actualizar feromona (1 = Actualizar solo 1 estado-accion, 2 = Actualizar el
	 * mejor camino, 3 = Actualizar los mejores w caminos): ";
	cin >> metodo;*/
	if(metodo < 1 || metodo > 3){
		cout << "Metodo no valido" << endl;
		cout << "Finalizando programa" << endl;
		return 0;
	}

// cout << "cargando la instancia..." << endl;

//a las cajas se les inicializan sus pesos en 1

	cout << "***** Creando el contenedor ****" << endl;
	cout << "File("<< format <<"): " << file << endl;
	cout << "Instance:" << inst+1 << endl;
	cout << "min_fr:" << min_fr << endl;
	cout << "Maxtime:" << maxtime << endl;

/*--------------------------------------FIN INICIACION DE VARIABLES-----------------------------------------*/

	double r=0.0;

	Block::FSB=fsb;
    clpState* s0 = new_state(file,inst, min_fr, 10000, f);


    cout << "n_blocks:"<< s0->get_n_valid_blocks() << endl;

    clock_t begin_time=clock();

    VCS_Function* vcs = new VCS_Function(s0->nb_left_boxes, *s0->cont,
    alpha, beta, gamma, p, delta, 0.0, r);
    vector< pair<double, double> > parameter_ranges(5);
    parameter_ranges[0]=make_pair(0.98, 1.02);
    parameter_ranges[1]=make_pair(0.0, 10.0);
    parameter_ranges[2]=make_pair(0.0, 10.0);
    parameter_ranges[3]=make_pair(0.0, 10.0);
    parameter_ranges[4]=make_pair(0.00, 0.2);

    tau_matrix tauM(parameter_ranges);
		tau_matrix::write_report=_write_report;

	cout << "greedy" << endl;
	SearchStrategy *gr = new Greedy(vcs, aco_alpha, aco_beta, &tauM);

	cout << "bsg" << endl;
	BeamACO *beamaco= new BeamACO(vcs,*gr, 8, 0.0, 0, _plot, aco_alpha, aco_beta,  &tauM, metodo, mod_factor, incremento);

	cout << "double effort" << endl;
    SearchStrategy *de= new DoubleEffort(*beamaco);

	cout << "copying state" << endl;
	State& s_copy= *s0->clone();

	list<State*> S;
	beamaco->timelimit=1000;

	beamaco->initialize (&s_copy);

	cout << "tunning" << endl;
	for(int i=0;i<0;i++){
		S.push_back(s0->clone());
		beamaco->next(S);
		beamaco->clean(S);

	}


	cout << "running" << endl;
	tauM.std_dev=std_dev;

    if(_plot)
    	de=beamaco;

    beamaco->update_ph=false;
    double eval=de->run(*s0->clone(), maxtime, begin_time) ;

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
