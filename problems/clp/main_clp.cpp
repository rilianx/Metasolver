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
    bool fsb=(atoi(argv[11])==1);
    bool kdtree=atoi(argv[12]);

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
    alpha, beta, gamma, p, delta, r);

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
    double eval = 1-gr->run(s_copy, max_time, begin_time) ;
	cout << eval << endl;


	/*const AABB* b = &dynamic_cast<const clpState*>(de->get_best_state())->cont.blocks->top();
	while(dynamic_cast<const clpState*>(de->get_best_state())->cont.blocks->has_next()){
		cout << *b << ":" << b->getVolume() << "(" << b->getOccupiedVolume() << ")" << endl;
		b = &dynamic_cast<const clpState*>(de->get_best_state())->cont.blocks->next();
	}
	 */

	dynamic_cast<const clpState*>(gr->get_best_state())->cont->MatLab_print();


}
