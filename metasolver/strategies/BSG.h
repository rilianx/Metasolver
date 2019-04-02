/*
 * BSGpath.h
 *
 *  Created on: 12 jul. 2017
 *      Author: iaraya
 */

#ifndef STRATEGIES_BSG_H_
#define STRATEGIES_BSG_H_

#include "../SearchStrategy.h"
#include <map>




namespace metasolver {



/**
 * Variante del beam search que utiliza una lista the best_path
 * para guiarse en la busqueda
 */
class BSG : public SearchStrategy {
public:
	/**
	 * Constructor
	 * @param greedy The underlying greedy algorithm
	 * @param expander
	 * @param beams the number of beams
	 * @p_elite the proportion of beams in the elite set (0.0, means 1 beam)
	 * @max_level_size the maximum number of expanded nodes by level of the tree
	 */
	BSG(ActionEvaluator* evl, SearchStrategy& greedy, int beams, double p_elite=0.0, int max_level_size=0, double diversity_threshold=0.0) :
		SearchStrategy(evl), greedy(greedy), beams(beams),
		max_level_size((max_level_size==0)? beams*beams:max_level_size),
		p_elite(p_elite), n_elite(max(1, (int)(p_elite*beams))), shuffle_best_path(false), diversity_threshold(diversity_threshold) {}


	virtual ~BSG();

    virtual void set_shuffle_best_path(bool b){
        shuffle_best_path=b;
    }

	/*
	 * Initialize the variables of the specific strategy
	 */
	virtual void initialize (State* s=NULL){
	    if(!s) return;
	    //The states of the first n_elite elements are modified by a first state
	    map<double, pair<State*, State*> >::iterator state_action=state_actions.begin();

        for(int i=0; state_action!=state_actions.end();i++){
           	if(i>=n_elite){
        	    delete state_action->second.second;
                state_action=state_actions.erase(state_action);
        	}else{
        	    state_action->second.first = s;
        	    if(shuffle_best_path) state_action->second.second->shuffle_path();
        	}
        }
	}

	/**
	 * Performs an iteration of the strategy
	 */
	virtual list<State*> next(list<State*>& S) ;


	virtual void clean(list<State*>& S){
		while(!S.empty()){ delete S.front(), S.pop_front(); }

	}

	/*
	 * Esta funcion "duplica" el esfuerzo de la estrategia.
	 * Si el esfuerzo es duplicado satisfactoriamente retorna true.
	 */
	virtual bool double_effort() {
	    int w= ((double)max_level_size/(double)beams) + 0.5;
		beams = double(beams)*sqrt(2) + 0.5;
		w=  double(w)*sqrt(2) +0.5;
		max_level_size = w*beams;
		n_elite = max(1, (int)(p_elite*beams));



		if(beams > 10000) return false;
		return true;
	}


protected:

	double diversity_threshold;
	list<State*> get_next_states(multimap<double, pair<State*, State*> >& sorted_states){

		list<State*> nextS;

		if(sorted_states.size()==0) return nextS;
		//para asociar los estados con a su diversidad
		multimap<double, multimap<double, pair<State*, State*> >::iterator> iterators;

		auto it=sorted_states.begin();
		for(;it!=sorted_states.end();it++){
			State* s= it->second.first;
			State* s1=it->second.second;
			Action* a = (s)? s->next_action(*s1):NULL;

			if(a){
				double div=1e19;
				for(auto it2=sorted_states.begin();it2!=it;it2++){
					State* s2=it2->second.second;
					double diff=s1->diff(*s2);
					if(diff<div)
						div=diff;

				}

				if(div==0) iterators.insert(make_pair(0,it));
				else iterators.insert(make_pair(diversity_threshold*div  -it->first,it));



				delete a;
			}else
				iterators.insert(make_pair(0,it));
		}

		for(auto it:iterators){
			if(it.first ==0 || sorted_states.size()>beams){
				it.second->second.first=NULL;
				delete it.second->second.second;
			    sorted_states.erase(it.second);
			}else{
				State* s= it.second->second.first;
				State* final_state=it.second->second.second;
				Action* a = (s)? s->next_action(*final_state):NULL;
				s=s->clone();
				it.second->second.first=s;
				s->transition(*a); delete a;
				nextS.push_back(s);

			}
		}

		return nextS;

	}

	multimap<double, pair<State*, State*> > state_actions;

	SearchStrategy& greedy;

	/**
	 * Number of beams
	 */
	int beams;

	/*
	 * Max number of expanded nodes in a level of the tree
	 */
	int max_level_size;

	double p_elite;
	int n_elite;

	bool shuffle_best_path;


};

} /* namespace clp */

#endif /* STRATEGIES_BSG_H_ */
