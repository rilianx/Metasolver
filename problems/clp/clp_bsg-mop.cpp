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

bool global::TRACE = false;

using namespace std;


int main(int argc, char** argv){

	string file=argv[1];
	int inst=atoi(argv[2]);
	double min_fr=atof(argv[3]);
	int max_time=atoi(argv[4]);

    double alpha=atof(argv[5]); //4.0
    double beta=atof(argv[6]); //1.0
    double gamma=atof(argv[7]); //0.2
    double p=atof(argv[8]); //0.04
    double delta=atof(argv[9]); //1.0
    double r=atof(argv[10]); //0.0

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

    cout << "cargando la instancia..." << endl;

    //a las cajas se les inicializan sus pesos en 1
    clpState* s0 = new_state(file,inst, min_fr, 10000, clpState::BR);

    cout << "n_blocks:"<< s0->get_n_valid_blocks() << endl;

    clock_t begin_time=clock();

    VCS_Function* vcs = new VCS_Function(s0->nb_left_boxes, *s0->cont,
    alpha, beta, gamma, p, delta);

    SearchStrategy *gr = new Greedy (vcs);

    BSG_MOP *bsg= new BSG_MOP(vcs,*gr, 4);

    SearchStrategy *de= new DoubleEffort(*bsg);

	State& s_copy= *s0->clone();

    double eval = 1-de->run(s_copy, max_time, begin_time) ;

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
