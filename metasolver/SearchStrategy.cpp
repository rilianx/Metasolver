/*
 * SearchStrategy.cpp
 *
 *  Created on: 19 oct. 2017
 *      Author: iaraya
 */



#include "SearchStrategy.h"




namespace metasolver {
/**
 * Retorna un puntero al primer elemento de la lista de mejores acciones
 */
Action* SearchStrategy::best_action(const State& s){
	list< Action* > actions;
	if(get_best_actions(s, actions,1)) return *actions.begin();
	else return NULL;
}

/*Action* SearchStrategy::best_action_ACO(const State& s){
	list< Action* > actions;
	if(get_best_actions(s, actions,1)) return *actions.begin();
	else return NULL;
}*/

int SearchStrategy::get_best_actions_ACO(const State& s, list< Action* >& bactions, int n){

	if(!evl) {
		cout << "The function State::get_best_actions should be implemented or an "
				<< "ActionEvaluator should be provided" << endl;
		exit(0);
	}

	list< Action* > actions;
	s.get_actions(actions);

	//if(actions.size()<=n) {bactions=actions; return bactions.size();}


	for(int i=0;i<n && actions.size()>0;i++){

		vector <double> sample_param_vector = tauM->sample_param_vector(&s);
		if(n>1) evl->set_parameter_values(sample_param_vector);

		double max_eval=0;
		list< Action* >::iterator best_action=actions.end();
		for(auto it=actions.begin(); it!=actions.end(); it++){
			double eval = evl->eval_action_rand(s,**it);
			if(eval>max_eval){
				max_eval=eval;
				best_action=it;
			}
		}

		if(best_action!=actions.end()){
			(*best_action)->parameter_values=sample_param_vector;
			(*best_action)->state_code=s.get_code();
			bactions.push_front( *best_action );
			actions.erase(best_action);
		}
	}

	for(auto a: actions){
		delete a;
	}

	return bactions.size();

}

int SearchStrategy::get_best_actions(const State& s, list< Action* >& bactions, int n){

	if(tauM) return get_best_actions_ACO(s, bactions, n);

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

	//escoge solo un set de valores para escoger todos los bloques
	vector <double> sample_param_vector = tauM->sample_param_vector(&s);
	if(sample_param_vector.size() > 0) evl->set_parameter_values(sample_param_vector);

	while(!actions.empty()){
		Action* a=actions.front(); actions.pop_front();
		double eval = evl->eval_action_rand(s,*a);
		if(eval>0 && (ranked_actions.size()<n || ranked_actions.begin()->first < eval)){
			a->parameter_values=sample_param_vector;
			a->state_code=s.get_code();
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
