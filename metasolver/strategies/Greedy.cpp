/*
 * Greedy.cpp
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

#include "Greedy.h"

namespace clp {

list<State*> Greedy::next(list<State*>& S) {
	State& s= **S.begin();

	Action* action = s.best_action();

	if(action){
		//std::cout << "selected box:" << action->block << endl;
		//std::cout << "selected space:" << action->space << endl;

		s.transition(*action);
		delete action;
		//cout << "valid_blocks:" << s.valid_blocks.size() << endl;
		//cout << (double)s.cont.getOccupiedVolume() / (double)s.cont.getVolume()  << endl;
		return S;
	}else{
		S.clear();
		return S;
	}
}

} /* namespace clp */
