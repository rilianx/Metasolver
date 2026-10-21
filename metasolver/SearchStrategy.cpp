/*
 * SearchStrategy.cpp
 *
 *  Created on: 19 oct. 2017
 *      Author: iaraya
 */



#include "SearchStrategy.h"




namespace metasolver {

Action* SearchStrategy::best_action(const State& s){
	list< Action* > actions;
	if(get_best_actions(s, actions,1)) return *actions.begin();
	else return NULL;
}

int SearchStrategy::get_best_actions(const State& s, list< Action* >& bactions, int n){
	return s.get_best_actions(evl, bactions, n);
}
}
