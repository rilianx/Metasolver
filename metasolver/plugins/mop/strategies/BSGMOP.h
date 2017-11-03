/*
 * BSGMOP.h
 *
 *  Created on: 13 sept. 2017
 *      Author: iaraya
 *      		MMoyano
 */

#ifndef PLUGINS_MOP_BSGMOP_H_
#define PLUGINS_MOP_BSGMOP_H_

#include <list>
#include "SearchStrategy.h"
#include "BSG.h"

using namespace std;
namespace metasolver {

/*
struct sort {
  bool operator() (const pair<double, double>& p1, const pair<double, double>& p2) const
  {
	  if(p1.first > p2.first) return true;

	  if(p1.first == p2.first && p1.second > p2.second) return true;

	  return false;

  }
};*/

class BSG_MOP : public BSG {
public:
	/**
	 * Constructor
	 * @param greedy The underlying greedy algorithm
	 * @param expander
	 * @param beams the number of beams
	 * @p_elite the proportion of beams in the elite set (0.0, means 1 beam)
	 * @max_level_size the maximum number of expanded nodes by level of the tree
	 */
	BSG_MOP(ActionEvaluator* evl, SearchStrategy& greedy, int beams, double p_elite=0.0, int max_level_size=0) :
		BSG(evl, greedy, beams, p_elite, max_level_size){

	}

    virtual ~BSG_MOP();

	/**
	 * Performs an iteration of the strategy
	 * @returns true if the search strategy has not finished yet
	 */
	virtual list<State*> next(list<State*>& S);

	map< pair<double, double>, State*> get_pareto_front(){
		return NDS;
	}

	/**
	 * TODO: the states in states are sorted by non-dominance  + crowding distance and the best n
	 * are inserted into filtered_states
	 *
	 * 1. Sort the solutions by applying the non-dominated sorting
	 * 2. while size(next frontier) + size(filtered_states) <= n the next frontier is copied into filtered_states
	 * 3. if size(next frontier) + size(filtered_states) > n, the frontier is sorted by crowding
	 * distance and only the first n - size(filtered_states) are copied to filtered_states
	 */

	/*
	 * Para cada par de states debemos hacer lo siguiente
	 	 	State* s= par.first;
			State* final_state=par.second;
			Action* a = (s)? s->next_action(*final_state):NULL;
	 * Antes de insertar el estado a filtered_states debemos "avanzar" al siguiente estado:
			//si a==NULL el estado no se inserta
	 		s=s->copy();
			s->transition(*a);
			par.first=s;

	 * Los estados finales no insertados en filtered_states deben ser eliminados:
		 	delete final_state;
	 * Las acciones (a) deben ser eliminadas: if(a) delete a;
	 */

	void filter_nondominated_sort (list< pair<State*,State*> >& filtered_states, int n);




	void function_sort(list< pair<pair<State*, State*>,double>>& crowding_list,int funtion);

	/**
	 * TODO: the states in frontier are sorted by crowding distance and the best n1
	 * are inserted into filtered_states
	 */
	void filter_crowding_distance (list< pair<State*, State*> >& frontera, list< pair<State*,State*> >& filtered_states, int n);
	//aqui suceda la magia del sur

	//void BSG_MOP::Non_Dominanted_sort(int N,list< pair<State*,State*> >& sorted_list);

private:

	//conjunto de soluciones no dominadas
	map< pair<double, double>, State*> NDS;


    //estados y caminos del beam para no perder soluciones
    map< pair<double, double>, pair<State*, State*> > state_actions;

	bool update(map< pair<double, double>, State*>& NDS, State& state_copy, double valuef1, double valuef2);
	void select_coeff(list<double>& coeff, int n);
};


} /* namespace clp */

#endif /* PLUGINS_MOP_BSGMOP_H_ */
