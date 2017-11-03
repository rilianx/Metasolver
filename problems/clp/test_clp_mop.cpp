/*
 * main_clp.cpp
 *
 *  Created on: 29 jun. 2017
 *      Author: iaraya
 */

#include <iostream>
//#include "objects/State.cpp"
#include "clpState.h"
#include "plugins/mop/MO_clpBasicEvaluator.h"
#include "VCS_Function.h"
#include "SpaceSet.h"
#include "Greedy.h"
#include "DoubleEffort.h"
#include "GlobalVariables.h"
#include "BSGMOP.h"

bool global::TRACE = false;

using namespace std;

//State* new_state(string file, int instance);

int main(int argc, char** argv){

	string file=argv[1];
	int inst=atoi(argv[2]);
	double min_fr=atof(argv[3]);
	int max_time=atoi(argv[4]);

    double alpha=atof(argv[5]); //0.0 - 10.0
    double beta=atof(argv[6]); //0.0 - 10.0
    double gamma=atof(argv[7]); //0.0 - 1.0
    double p=atof(argv[8]); //0.0 - 0.1
    double delta=atof(argv[9]); //0.0 - 10.0
    double r=atof(argv[10]);

    cout << endl <<  "******* Parameters *********" << endl;
    cout << "file:" << file << "(instance: " << inst << ")" <<  endl;
    cout << "min_fr:" << min_fr << endl;
    cout << "max_time:" << max_time << endl;
    cout << "alpha:" << alpha << endl;
    cout << "beta:" << beta << endl;
    cout << "gamma:" << gamma << endl;
    cout << "p:" << p << endl;
    cout << "delta:" << delta << endl;
    cout << "r:" << r << endl;
    cout << "*****************************" << endl << endl;

	srand(1);

	//SpaceSet::random_spaces=true;

    cout << "cargando la instancia..." << endl;

    //a las cajas se les inicializan sus pesos en 1
    clpState* s0 = new_state(file,inst, min_fr, 10000, clpState::BR);

    cout << "n_blocks:"<< s0->get_n_valid_blocks() << endl;

    clock_t begin_time=clock();

    VCS_Function* vcs = new VCS_Function(s0->nb_left_boxes, *s0->cont,
    alpha, beta, gamma, p, delta);

    //MO_clpBasicEvaluator evl(*vcs, r);

    SearchStrategy *gr = new Greedy (vcs);

    BSG_MOP *bsg= new BSG_MOP(vcs,*gr, 4);

    SearchStrategy *de= new DoubleEffort(*bsg);

	State& s_copy= *s0->clone();

   // cout << s0.valid_blocks.size() << endl;

    double eval = 1-bsg->run(s_copy, max_time, begin_time) ;

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
