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


int main(int argc, char** argv){

	string file=argv[1];
	int inst=atoi(argv[2]);
	double min_fr=atof(argv[3]); //<-- 0.98 o 1.0

	int max_time=atoi(argv[4]); //500
    double alpha=atof(argv[5]); //4.0
    double beta=atof(argv[6]); //1.0
    double gamma=atof(argv[7]); //0.2
    double p=atof(argv[8]); //0.04
    double delta=atof(argv[9]); //1.0
	double f=atof(argv[10]); //0.0
    double r=atof(argv[11]); //0.0
    bool fsb=(atoi(argv[12])==1); //0
    bool kdtree=atoi(argv[13]); //0

	srand(1);



	//SpaceSet::random_spaces=true;
    //global::TRACE=true;

    cout << "cargando la instancia..." << endl;

    Block::FSB=fsb;
    clpState* s0 = new_state(file,inst, min_fr, 10000);

    if(kdtree)
       s0 = new clpState_kd(*s0);

    cout << "n_blocks:"<< s0->get_n_valid_blocks() << endl;

    clock_t begin_time=clock();

    VCS_Function* vcs = new VCS_Function(s0->nb_left_boxes, *s0->cont,
    alpha, beta, gamma, p, delta, f, r);

	if(kdtree){
		kd_block::set_vcs(*vcs);
		kd_block::set_alpha(alpha);
		kd_block::set_alpha(p);
	}

	//for(int i=0;i<10000; i++)
	//	exp->best_action(*s0);

	cout << "greedy" << endl;
    SearchStrategy *gr = new Greedy (vcs);

	cout << "bsg" << endl;
    BSG *bsg= new BSG(vcs,*gr, 4);
    //BSG_midBSG *bsg= new BSG_midBSG(*gr, *exp, 4);

    //bsg->set_shuffle_best_path(true);

	cout << "double effort" << endl;
    SearchStrategy *de= new DoubleEffort(*bsg);

	cout << "copying state" << endl;
	State& s_copy= *s0->clone();

   // cout << s0.valid_blocks.size() << endl;

	cout << "running" << endl;
    double eval = 1-de->run(s_copy, max_time, begin_time) ;
	cout <<1- eval << endl;


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


	}*/

/*
	const AABB* b = &dynamic_cast<const clpState*>(de->get_best_state())->cont->blocks->top();
	while(dynamic_cast<const clpState*>(de->get_best_state())->cont->blocks->has_next()){
		cout << *b << ":" << b->getVolume() << "(" << b->getOccupiedVolume() << ")" << endl;
		b = &dynamic_cast<const clpState*>(de->get_best_state())->cont->blocks->next();
	}
*/

	//s00->cont->MatLab_print();


}
