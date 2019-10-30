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
	double val = s.get_value();

	double max_value = -1.0 ;
	double best_pred = 0.0 ;

	list< Action* > actions;
	get_best_actions(s, actions, n);

	for(auto action:actions){
		State* s_copy = s.clone();
		s_copy->transition(*action);
		delete action;

		double val_diff = 	s_copy->get_value()-val;
		double pred = s_copy->ann_prediction();
		cout << "pred, value: " <<  pred << ",";

    double value;
		if(prev_pred>0) value = pred-(prev_pred-pred)*((pred-val)/val_diff);
		else value = pred;
		cout << value << endl;

		if(value > max_value){
			if(best_state && best_state!=&s) delete best_state;
			max_value=value;
			best_state=s_copy;
			best_pred=pred;
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
	prev_pred=best_pred;
	return S;
}

} /* namespace clp */
