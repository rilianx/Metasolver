/*
 * Expander.cpp
 *
 *  Created on: 5 jul. 2017
 *      Author: iaraya
 */

#include<iostream>
#include "../metasolver/Expander.h"
#include "GlobalVariables.h"

namespace clp {

int ExpanderHF::best_actions(State& s, int n, list< Action* >& bactions){



	//the best n actions ranked from worst to best
	multimap<double,Action*> ranked_actions;
	//map<double,Action*> ranked_actions;

	list< Action* > actions;
	s.get_actions(actions, n);

	//if(actions.size()<=n) {bactions=actions; return bactions.size();}

	while(!actions.empty()){
		Action* a=actions.front(); actions.pop_front();
		double eval = evl.eval_action_rand(s,*a);
		if(eval>0 && (ranked_actions.size()<n || ranked_actions.begin()->first < eval)){
			ranked_actions.insert(make_pair(eval,a));
			if(ranked_actions.size()==n+1) {
				delete (ranked_actions.begin()->second);
				ranked_actions.erase(ranked_actions.begin());
			}
		}else delete a;
	}

	//se colocan las acciones en la lista en el orden inverso
	while(ranked_actions.size()>0){
		//cout << "eval:" << ranked_actions.begin()->first << endl;
		bactions.push_front( ranked_actions.begin()->second );
		ranked_actions.erase(ranked_actions.begin());
	}


	return bactions.size();
}


} /* namespace clp */
