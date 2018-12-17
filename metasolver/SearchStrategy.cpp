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
	if(aco_beta>0.0) return get_best_actions_aco(s, bactions, n);

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

int SearchStrategy::get_best_actions_aco(const State& s, list< Action* >& bactions, int n){

	if(!evl) {
		cout << "The function State::get_best_actions_prob should be implemented or an "
				<< "ActionEvaluator should be provided" << endl;
		exit(0);
	}

	list< pair<double,Action*> > evaluated_actions;
	//map<double,Action*> ranked_actions;

	list< Action* > actions;
	s.get_actions(actions);

	//if(actions.size()<=n) {bactions=actions; return bactions.size();}


  double sum_eval=0;
	while(!actions.empty()){
		Action* a=actions.front(); actions.pop_front();
		//double eval = evl->eval_action_rand(s,*a);  (evaluacion normal)
		double eval = pow(tauM.get_tau(&s, a),aco_alpha) *  pow(evl->eval_action_rand(s,*a),aco_beta); //formula hormigas?

		evaluated_actions.push_back(make_pair(eval,a));
		sum_eval += eval;
	}

	//se colocan las acciones en la lista en el orden inverso
	while(evaluated_actions.size()>0 && bactions.size()<n){
		//cout << "eval:" << ranked_actions.begin()->first << endl;
		double r = (rand()/(double)RAND_MAX)*sum_eval;
		if(r>sum_eval) r=sum_eval;

		double roulette=0.0;
		auto it=evaluated_actions.begin();

		for(; it!=evaluated_actions.end(); it++){
			roulette += it->first;
			if(roulette >= r)
				break;
		}
		if(it==evaluated_actions.end()) it--;

		sum_eval-=it->first;
		bactions.push_front( it->second );
		evaluated_actions.erase(it);
	}

	for(auto act:evaluated_actions)
	   delete act.second;

	return bactions.size();

}

}
