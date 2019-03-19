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
#include "mclp-state.h"
#include "BlockSet.h"
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

int solve(Greedy* gr, BSG *bsg, mclpState* s0, int maxtime, clock_t begin_time){
	mclpState::initalize_priorities();
	//multimap<double, map<const BoxShape*, int> > bins;
	list < pair <double, map<const BoxShape*, int>> > bins;
	map<const BoxShape*, int> used_boxes;
	int box_quantity = 0;

	for(int i=0; i<1000; i++){
		//copia el estado base
		mclpState& s_copy= *dynamic_cast<mclpState*>(s0->clone());

		//filtrado de cajas
	    s_copy.select_boxes();

		//usa greedy para llenar contenedor
		double eval=gr->run(s_copy, maxtime, begin_time);

		//se seleccionan cajas con mayor prioridad, sin contar las que se hayan colocado en el bin actual
		s_copy.select_boxes(&dynamic_cast<const mclpState*>(gr->get_best_state())->cont->nb_boxes);
		eval=gr->run(s_copy, maxtime, begin_time);

		//dynamic_cast<const mclpState*>(gr->get_best_state())->cont->nb_boxes)
		//todas las cajas: s0->nb_left_boxes


		//se actualizan las prioridades
		dynamic_cast<const mclpState*>(gr->get_best_state())->update_priorities(0.98,s0->nb_left_boxes);

		//se almacena el bin en el mapa
		bins.push_back(make_pair(eval, dynamic_cast<const mclpState*>(gr->get_best_state())->cont->nb_boxes));

		//cout << eval << ":" ;
		for(auto box:dynamic_cast<const mclpState*>(gr->get_best_state())->cont->nb_boxes){
			cout << box.first->get_id() << "(" << box.second << "),";
			used_boxes[box.first]++;
		}
		cout << endl;

		//dynamic_cast<const mclpState*>(gr->get_best_state())->cont->MatLab_print();

	}

	int high_quantity = 0;

	cout << "used_boxes" << endl;
	for(auto box: used_boxes){
		cout << box.first->get_id() << "(" << box.second << ")," ;
		if(box.second > high_quantity) {
			high_quantity = box.second;
		}
	}



	map<const BoxShape*, int> dont_used_boxes;

	for(auto box: used_boxes){
		dont_used_boxes.insert(pair<const BoxShape*, int>(box.first, high_quantity - box.second));
		box_quantity += high_quantity - box.second;
	}

	cout << "\n\ndont_used_boxes" << endl;
	for(auto box: dont_used_boxes){
		cout << box.first->get_id() << "(" << box.second << ")," ;
	}

	return bins.size();

	cout << endl;
	cout << "\nhigh_quantity" << endl;
	cout << high_quantity << endl;

	cout << "\nbox_quantity" << endl;
	cout << box_quantity << "\n" << endl;

	used_boxes.clear();

	while(box_quantity > 0){
		//copia el estado base
		mclpState& s_copy= *dynamic_cast<mclpState*>(s0->clone());

		//se seleccionan cajas con mayor prioridad, sin contar las que se hayan colocado en el bin actual

		s_copy.select_boxes(&dont_used_boxes, false);//&dynamic_cast<const mclpState*>(gr->get_best_state())->cont->nb_boxes);
		double eval=gr->run(s_copy, maxtime, begin_time);

		//dynamic_cast<const mclpState*>(gr->get_best_state())->cont->nb_boxes)
		//todas las cajas: s0->nb_left_boxes

		//se almacena el bin en el mapa
		bins.push_back(make_pair(eval, dont_used_boxes));

		for(auto box:dynamic_cast<const mclpState*>(gr->get_best_state())->cont->nb_boxes){
			//cout << box.first->get_id() << ",";
			cout << box.first->get_id() << "(" << box.second << ")," ;
			box_quantity -= box.second;
			if(dont_used_boxes[box.first] - box.second > 0)
				dont_used_boxes[box.first] = dont_used_boxes[box.first] - box.second;
			else
				dont_used_boxes[box.first] = 0;
		}
		cout << endl;
	}

	cout << "\n\nused_boxes" << endl;
	for(auto box: dont_used_boxes){
		cout << box.first->get_id() << "(" << box.second << ")," ;
	}

	cout << endl;

	return bins.size();


	//cout << nb_bins << endl;

	//Se escogen dos bins y se llenan en secuencia
	//Si no sobran cajas y la diferencia entre bins se incrementa se acepta el cambio

	//Si un bin queda sin cajas se elimina

	/*
	for(int j=0; j<500; j++){
		int r1=rand()%(bins.size());
		int r2=rand()%(bins.size());
		while(r2==r1) r2=rand()%(bins.size());
		multimap<double, map<const BoxShape*, int> >::iterator it1 = bins.begin();
		advance (it1, r1);
		multimap<double, map<const BoxShape*, int> >::iterator it2 = bins.begin();
		advance (it2, r2);

		double diff=abs(it1->first-it2->first);

		map<const BoxShape*, int> boxes1=it1->second;
		map<const BoxShape*, int> boxes2=it2->second;
		for(auto b:boxes1)
			boxes2[b.first]+=b.second;

		bsg->initialize();
		mclpState& s_copy= *dynamic_cast<mclpState*>(s0->clone());
		s_copy.set_boxes(boxes2);

		double eval1=bsg->run(s_copy, maxtime, begin_time);
		auto nb_boxes1=dynamic_cast<const mclpState*>(bsg->get_best_state())->cont->nb_boxes;

		for(auto b:nb_boxes1){
			boxes2[b.first]-=b.second;
			if(boxes2[b.first]==0) boxes2.erase(b.first);
		}



		if(boxes2.empty()){
			cout << "bin removed!" << endl;
			bins.erase(it1);
			bins.erase(it2);
			bins.insert(make_pair(eval1, nb_boxes1));
			continue;
		}

		bsg->initialize();
		mclpState& s_copyy= *dynamic_cast<mclpState*>(s0->clone());
		s_copyy.set_boxes(boxes2);

		double eval2=gr->run(s_copyy, maxtime, begin_time);
		auto nb_boxes2=dynamic_cast<const mclpState*>(gr->get_best_state())->cont->nb_boxes;

		for(auto b:nb_boxes2){
			boxes2[b.first]-=b.second;
			if(boxes2[b.first]==0) boxes2.erase(b.first);
		}

		if(!boxes2.empty() || abs(eval1-eval2) <= diff)	continue;

		//cout << abs(eval1-eval2) << "," << diff << endl;
		bins.erase(it1);
		bins.erase(it2);
		bins.insert(make_pair(eval1, nb_boxes1));
		bins.insert(make_pair(eval2, nb_boxes2));


	}
*/

	return bins.size();



}

int main(int argc, char** argv){

	args::ArgumentParser parser("********* BSG-CLP *********.", "BSG Solver for CLP.");
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::ValueFlag<int> _inst(parser, "int", "Instance", {'i'});
	args::Flag _rotate(parser, "rotate", "rotations are allowed?", {"rot"});
	args::ValueFlag<int> _nboxes(parser, "int", "Number of boxes for each type", {"nboxes"});
	args::ValueFlag<double> _min_fr(parser, "double", "Minimum volume occupied by a block (proportion)", {"min_fr"});
	args::ValueFlag<int> _maxtime(parser, "int", "Timelimit", {'t', "timelimit"});
	args::ValueFlag<int> _seed(parser, "int", "Random seed", {"seed"});
	args::ValueFlag<double> _alpha(parser, "double", "Alpha parameter", {"alpha"});
	args::ValueFlag<double> _beta(parser, "double", "Beta parameter", {"beta"});
	args::ValueFlag<double> _gamma(parser, "double", "Gamma parameter", {"gamma"});
	args::ValueFlag<double> _delta(parser, "double", "Delta parameter", {"delta"});
	args::ValueFlag<double> _p(parser, "double", "p parameter", {'p'});
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

	string file=_file.Get();
	int inst=(_inst)? _inst.Get():0;
	double min_fr=(_min_fr)? _min_fr.Get():0.98;
	int maxtime=(_maxtime)? _maxtime.Get():100;

	double alpha=3.0, beta=2.0, gamma=0.5, delta=1.0, p=0.04, maxtheta=0.0;
	int nboxes=1;
	if(_maxtime) maxtime=_maxtime.Get();
	if(_alpha) alpha=_alpha.Get();
	if(_beta) beta=_beta.Get();
	if(_gamma) gamma=_gamma.Get();
	if(_delta) delta=_delta.Get();
	if(_p) p=_p.Get();
	if(_nboxes) nboxes=_nboxes.Get();

	int seed=(_seed)? _seed.Get():1;
	srand(seed);

	global::TRACE = trace;

 cout << "cargando la instancia..." << endl;

//a las cajas se les inicializan sus pesos en 1

	cout << "***** Creando el contenedor ****" << endl;
	cout << "Instance:" << inst+1 << endl;
	cout << "min_fr:" << min_fr << endl;
	cout << "Maxtime:" << maxtime << endl;

    mclpState* s0 = new_mstate(file,inst, min_fr, 10000, _rotate, nboxes);

    cout << "n_blocks:"<< s0->get_n_valid_blocks() << endl;

    clock_t begin_time=clock();


    VCS_Function* vcs = new VCS_Function(s0->nb_left_boxes, *s0->cont,
        alpha, beta, gamma, p, delta, 1.0, 0.0);

		cout << "greedy" << endl;
		Greedy *gr = new Greedy (vcs);

		cout << "bsg" << endl;
		BSG *bsg= new BSG(vcs,*gr, 12, 0.0, 0, _plot);
		bsg->trace=false;


   	int bins=solve(gr, bsg, s0, maxtime, begin_time);
	cout << bins << endl;

	//if(_plot){
	//   pointsToTxt(&s_copy, 0);
	//   system("firefox problems/clp/tree_plot/index.html");
	//}

}
