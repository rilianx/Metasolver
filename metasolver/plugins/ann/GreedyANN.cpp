/*
 * GreedyANN.cpp
 *
 *  Created on: Sep 25, 2019
 *      Author: iaraya
 */

#include "GreedyANN.h"


namespace metasolver {

list<State*> GreedyANN::next(list<State*>& S) {
	State& s= **S.begin();
	double max_value = -1.0 ;

	list< Action* > actions;
	get_best_actions(s, actions, n);

	for(auto action:actions){
		State* s_copy = s.clone();
		s_copy->transition(*action);
		delete action;

		double value = s_copy->ann_prediction();
		cout << value << endl;
		if(value > max_value){
			if(best_state && best_state!=&s) delete best_state;
			max_value=value;
			best_state=s_copy;
		}else
			delete s_copy;


	}

	if(actions.size()>0){
		S.clear();
		if(best_state!=&s) delete &s;
		S.push_back(best_state);
		cout << best_state->get_value() << endl;
	}else{
		S.clear();
	}
	return S;
}

} /* namespace clp */
