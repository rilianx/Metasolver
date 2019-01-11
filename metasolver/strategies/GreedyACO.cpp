/*
 * GreedyACO.cpp
 *
 *  Created on: 20 dic. 2018
 *      Author: emiliano
 */

#include "GreedyACO.h"

namespace metasolver {

list<State*> GreedyACO::next(list<State*>& S) {
	State& s= **S.begin();

	Action* action = best_action(s);

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
		//get path.length()
	}
}

} /* namespace clp */


