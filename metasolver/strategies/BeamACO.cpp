/* s
 * BSGpath.cpp
 *
 *  Created on: 12 jul. 2017
 *      Author: iaraya
 */

#include "BeamACO.h"

namespace metasolver {



BeamACO::~BeamACO(){

}

list<State*> BeamACO::next(list<State*>& S){
    

     //no hay estados en el arbol
	 if(S.size()==0) return S;

     //se expanden la cantidad de estados de la lista S
     int i=0;
     int w=0;
     for(list<State*>::iterator itS=S.begin(); itS!=S.end() && get_time()<=timelimit; itS++,i++){
    	 State& state=**itS;

    	 /*Si se encuentra en el nodo raiz, entonces imprime el taman~o de caminos por nodo y la
    	 cantidad de expanciones maxima que tendra un nivel*/
         if(state.is_root()){
        	 cout << "beams/max_level_size:" << beams << "/" << max_level_size << endl;
         }

         //se obtiene la lista de las mejores acciones a partir del estado actual
         list< Action* > best_actions;
         
         //en cada nivel del arbol de busqueda deberia explorar los nodos max_level_size, entonces..
         w =  (double) max_level_size / (double) S.size() + 0.5;
         get_best_actions(state, best_actions, w);

         //Las acciones son evaluadas y guardadas en el mapa ordenado
         list< Action* >::iterator it = best_actions.begin();

         for(; it!=best_actions.end()&& get_time()<=timelimit;it++){

        	 State& state_copy = *state.clone();
        	 state_copy.transition(**it);
        	 delete *it;

             double value = greedy.run(state_copy, timelimit, begin_time);

            //best_state update
             if(value > get_best_value()){
            	 if(best_state) delete best_state; 
            	 best_state = state_copy.clone();
            	 cout << "[BeamACO] new best_solution_found ("<< get_time() <<"): " << value << " "
            			 << best_state->get_path().size() << " nodes" << endl;
             }


             if(state_actions.find(-value)==state_actions.end()){
            	 state_actions[-value]= make_pair(&state, &state_copy);
             }else delete &state_copy;

         }
     }

     //actualiza feromona
    i=0;
	for(auto state_action:state_actions){

		if(i==beams) break;

		State* s= state_action.second.first;
		if(!s) break;

		State* final_state=state_action.second.second;


		//si simulacion del hijo mejora al padre
		if(-state_action.first > s->sim_value){
			list< const Action* > path= final_state->get_path();
			list< const Action* >::iterator it=path.begin();
			advance(it,s->get_path().size());
			path.erase(path.begin(),it);

			for(auto a:path)
				tauM->add_pheromone(a->state_code,a->parameter_values);
		}



		if(metodo == ONE_PATH)	break;
		i++;
	}

  	list<State*> l=get_next_states(state_actions);

    return l;
}
} /* namespace clp */
