/*
 * State.cpp
 *
 *  Created on: 4 sept. 2018
 *      Author: iaraya
 */


#include "State.h"

namespace metasolver {

int State::count_states=0;

ostream & operator << (ostream &out, const Action &a){
	a.serialize(out);
	return out;
}

Action* State::next_action(State& final){
    if(get_path().size() >= final.get_path().size() ) return NULL;

    list< const Action* >::iterator act=final.get_path().begin();
    advance(act,get_path().size());

	return (*act)->clone();
}

}
