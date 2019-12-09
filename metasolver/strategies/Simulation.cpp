/*
 * BSGpath.cpp
 *
 *  Created on: 12 jul. 2017
 *      Author: iaraya
 */

#include "Simulation.h"

namespace metasolver {



Simulation::~Simulation(){

}

list<State*> Simulation::next(list<State*>& S){

	int a;
	if(actions.empty()) a=1;
	else {
		a= actions.front(); actions.pop_front();
	}

	State& s= **S.begin();

	Action* action = NULL;

    list< Action* > best_actions;
    int* tot_act=new int;
    int n = get_best_actions(**S.begin(), best_actions, a, tot_act);

    cout << a << "/" << *tot_act << ";";
    delete tot_act;

    if(n>=a)
    	action = best_actions.back();


	if(action){
		s.transition(*action);
		delete action;
		return S;
	}else{
		cout << endl;
		S.clear();
		return S;
	}
}


} /* namespace clp */
