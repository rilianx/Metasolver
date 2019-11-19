/*
 * GreedyANN.cpp
 *
 *  Created on: Sep 25, 2019
 *      Author: iaraya
 */

#include "GreedyANN_C.h"


namespace metasolver {

list<State*> GreedyANN_C::next(list<State*>& S) {
	State& s= **S.begin();
	double val = s.get_value();

	if(!random){
		double conf = s.ann_prediction();
		evl->use_configuration((int)(conf+0.5));
	}else
		evl->use_configuration(rand()%3);

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
	}
}

} /* namespace clp */
