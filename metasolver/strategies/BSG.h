/*
 * BSGpath.h
 *
 *  Created on: 12 jul. 2017
 *      Author: iaraya
 */

#ifndef STRATEGIES_BSG_H_
#define STRATEGIES_BSG_H_

#include <cmath>
#include <set>
#include <functional>
#include "../SearchStrategy.h"

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
	 * @verbose prints the new best solutions
	 */
	BSG(ActionEvaluator* evl, SearchStrategy& greedy, int beams, double p_elite=0.0, int max_level_size=0, bool verbose=true) :
		SearchStrategy(evl), greedy(greedy), beams(beams),
		max_level_size((max_level_size==0)? beams*beams:max_level_size),
		p_elite(p_elite), n_elite(max(1, (int)(p_elite*beams))), verbose(verbose), root_samples(1), root_inherit(false), max_carried(1.0) {}


	virtual ~BSG();

	/*
	 * Initialize the variables of the specific strategy
	 */
	virtual void initialize (State* s=NULL){
	    if(!s){
	    	if(best_state) {delete best_state; best_state=NULL;}

	    	map<double, pair<State*, State*> >::iterator state_action=state_actions.begin();
	    	for(int i=0; state_action!=state_actions.end();i++) {
	    		delete state_action->second.second;
	    		state_action=state_actions.erase(state_action);
	    	}
	    	return;
	    }
	    //The states of the first n_elite elements are modified by a first state
	    map<double, pair<State*, State*> >::iterator state_action=state_actions.begin();

        for(int i=0; state_action!=state_actions.end();i++){
           	if(i>=n_elite){
        	    delete state_action->second.second;
                state_action=state_actions.erase(state_action);
        	}else{
        	    state_action->second.first = s;
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

	void set_beams(int b){beams=b;}

	/**
	 * Evaluacion de los hijos de la raiz con m variantes del greedy: variant(s, j) modifica
	 * el estado s para la variante j (j=0: la original; j=-1: volver a la original).
	 * El valor del hijo es el maximo sobre las variantes y se sigue el camino de la mejor;
	 * si inherit es true, el hijo continua con esa variante (si no, con la original)
	 */
	void set_root_samples(int m, std::function<void(State&, int)> variant, bool inherit){
		root_samples=m; root_variant=variant; root_inherit=inherit;
	}

	/**
	 * Limita la proporcion del beam que pueden ocupar los caminos heredados de
	 * niveles anteriores (cuyo valor ya fue seleccionado como maximo, por lo que
	 * tienen ventaja frente a los hijos evaluados con una sola corrida greedy)
	 */
	void set_max_carried(double q){max_carried=q;}


protected:

	template<class map_container>
	list<State*> get_next_states(map_container& sorted_states){
		list<State*> nextS;
		typename map_container::iterator state_action=sorted_states.begin();

		//Para cada state->final_state se rescata la accion
		//Si no hay acción posible o si la cuota the beams ha sido sobrepasada
		//se elimina final_state y el elemento del mapa
		int k=0;
		int n_carried=0;
		while(state_action!=sorted_states.end()){
			State* s= state_action->second.first;
			State* final_state=state_action->second.second;
			Action* a = (s)? s->next_action(*final_state):NULL;

			//cuota de caminos heredados de niveles anteriores
			bool carried = fresh.find(final_state)==fresh.end();
			if(a && carried && nextS.size()<beams && n_carried>=max(1, (int)(max_carried*beams+0.5))){
				delete a; a=NULL;
			}

			if(nextS.size()<beams && a){
				if(carried) n_carried++;
				s=s->clone();
				state_action->second.first=s;
				s->transition(*a);
				s->inherit(*final_state);
				nextS.push_back(s);

		 		//cout <<  state_action->second.first->get_value() << " --> " << state_action->second.second->get_value()  << endl;


			}else state_action->second.first=NULL;

			//other elements are removed from the state_actions
			 if(k>=beams){
				delete final_state;
			   state_action=sorted_states.erase(state_action);
			 }else state_action++;

		 if(a) delete a;
		 k++;
		}

		fresh.clear();
		return nextS;

	}

	map<double, pair<State*, State*> > state_actions;

	//estados finales (greedy) evaluados en el nivel actual; los demas de
	//state_actions son caminos heredados de niveles anteriores
	set<const State*> fresh;

	int root_samples;
	std::function<void(State&, int)> root_variant;
	bool root_inherit;

	//proporcion maxima del beam para caminos heredados (1: sin limite)
	double max_carried;

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

	//imprime las nuevas mejores soluciones
	bool verbose;

};

} /* namespace clp */

#endif /* STRATEGIES_BSG_H_ */
