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

	if(!evl) {
		cout << "The function State::get_best_actions should be implemented or an "
				<< "ActionEvaluator should be provided" << endl;
		exit(0);
	}
	//the best n actions ranked from worst to best
	multimap<double,Action*> ranked_actions;
	//map<double,Action*> ranked_actions;

	list< Action* > actions;
	s.get_actions(actions);

	//if(actions.size()<=n) {bactions=actions; return bactions.size();}

	while(!actions.empty()){
		Action* a=actions.front(); actions.pop_front();
		double eval = evl->eval_action_rand(s,*a);
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
}
