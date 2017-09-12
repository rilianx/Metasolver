/*
 * main_clp.cpp
 *
 *  Created on: 29 jun. 2017
 *      Author: iaraya
 */

#include <iostream>
//#include "objects/State.cpp"
#include "clp/clpState.h"
#include "clp/plugins/kdtree/clpStatekd.h"
#include "clp/plugins/kdtree/BlockSet.h"
#include "metasolver/strategies/BSG_midBSG.h"
#include "clp/evaluators/VCS_Function.h"
#include "clp/evaluators/VCS_Function.h"
#include "clp/objects2/SpaceSet.h"
#include "metasolver/strategies/Greedy.h"
#include "metasolver/strategies/BSGpath.h"
#include "metasolver/strategies/DoubleEffort.h"
#include "metasolver/GlobalVariables.h"

bool global::TRACE = false;

using namespace std;

//State* new_state(string file, int instance);



int best_cut(clpState* s){

	long cut=0;
	set<int> promising_cuts;
	//se recorre la lista de bloques del contenedor
	//y se obtienen todos los cortes
	const AABB* b=&s->cont.blocks->top();
	while(b){
		promising_cuts.insert(b->getXmin());
		promising_cuts.insert(b->getXmax());
		if(s->cont.blocks->has_next())
			b=&s->cont.blocks->next();
		else b=NULL;
	}

    list<const AABB*> blocks = s->cont.blocks->get_bisected_objects(cut);

	return 0;
}


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
    bool kdtree=atoi(argv[11]);

	srand(1);



	//SpaceSet::random_spaces=true; 

    cout << "cargando la instancia..." << endl;
    clpState* s0 = new_state(file,inst, min_fr, 10000);

    if(kdtree)
       s0 = new clpState_kd(*s0);

    cout << "n_blocks:"<< s0->get_n_valid_blocks() << endl;

    clock_t begin_time=clock();

    VCS_Function* vcs = new VCS_Function(s0->nb_left_boxes, s0->cont,
    alpha, beta, gamma, p, delta, r);

	Expander* exp ;
	if(!kdtree)
		exp = new ExpanderHF (*vcs);
	else{
		exp = new Expander ();
		kd_block::set_vcs(*vcs);
		kd_block::set_alpha(alpha);
		kd_block::set_alpha(p);
	}

	//for(int i=0;i<10000; i++)
	//	exp->best_action(*s0);


    SearchStrategy *gr = new Greedy (*exp);

    BSG_path *bsg= new BSG_path(*gr, *exp, 4);
    //BSG_midBSG *bsg= new BSG_midBSG(*gr, *exp, 4);

    //bsg->set_shuffle_best_path(true);

    SearchStrategy *de= new DoubleEffort(*bsg);

	State& s_copy= *s0->copy(true);
 
   // cout << s0.valid_blocks.size() << endl;

	global::n_eval=0;
    double eval = 1-bsg->run(s_copy, max_time, begin_time) ;
	cout << global::n_eval << endl;
	cout << eval << endl;
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
