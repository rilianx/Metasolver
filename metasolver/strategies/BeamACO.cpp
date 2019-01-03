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
    


	//cout << "next" << endl;
     //no hay mas estados en el arbol
	 if(S.size()==0) return S;

     //se expanden los nodos de la lista S
     int i=0;
     int w=0;
     for(list<State*>::iterator itS=S.begin(); itS!=S.end() && get_time()<=timelimit; itS++,i++){
    	 State& state=**itS;
         //cout << state.get_value() << endl;
    	 //Si se encuentra en el nodo raiz, entonces imprime el taman~o de caminos por nodo y la
    	 //cantidad de expanciones maxima que tendra un nivel
         if(state.is_root()){
        	 cout << "beams/max_level_size:" << beams << "/" << max_level_size << endl;
         }


         //se obtiene la lista de las mejores acciones a partir del estado actual
         list< Action* > best_actions;
         
         //en cada nivel del arbol de busqueda deberia explorar los nodos max_level_size, entonces..
         w =  (double) max_level_size / (double) S.size() + 0.5;
         //cout << w << " and " << S.size() << " and " << max_level_size << endl;
         get_best_actions_aco(state, best_actions, w);



         //the actions are evaluated and saved in the sorted map
         list< Action* >::iterator it = best_actions.begin();
         //cout << state_actions.size() << endl;

         for(; it!=best_actions.end()&& get_time()<=timelimit;it++){

        	 State& state_copy = *state.clone();
        	 state_copy.transition(**it);
        	 //cout << state_copy.get_value() << endl;
        	 delete *it;

             double value = greedy.run(state_copy, timelimit, begin_time);
             //cout << value << endl;


            //best_state update
             if(value > get_best_value()){
            	 if(best_state) delete best_state; 
            	 best_state = state_copy.clone();
            	 cout << "[BeamACO] new best_solution_found ("<< get_time() <<"): " << value << " "
            			 << best_state->get_path().size() << " nodes" << endl;
             }


             if(state_actions.find(-value)==state_actions.end()){
            	 //cout << value << endl;
            	 //marcar estados nuevos
            	 state.marked = true;
            	 state_actions[-value]= make_pair(&state, &state_copy);


             }else delete &state_copy;

         }
     }

     //actualiza feromona
    i=0;
    //tauM->update_factor(0.87);
		for(auto state_action:state_actions){
			//cout << w << endl;
			if(i==w) {
				break;
			}
			State* s= state_action.second.first;
			if(!s) break;
			State& state_copy2 = *s->clone();
			State* final_state=state_action.second.second;

			if(s->marked == true){
				while(true){
					Action* a = (s)? state_copy2.next_action(*final_state):NULL;
					//pair<long, long> p = s->get_code(*a);
					//cout << std::get<0>(p) << " and " << std::get<1>(p) << endl;
					//Action* b = (s)? s->next_action(*final_state):NULL;
					if(a){

						//cout << "///////////" << endl;
						//cout << tauM->get_tau(&state_copy2,a) << endl;

						tauM->incr_tau(&state_copy2,a,0.1);

						//tauM->update_factor(0.99);
						//cout << tauM->get_tau(&state_copy2,a) << endl;
						//cout << "///////////" << endl;
						state_copy2.transition(*a);
						delete a;
						if(metodo == 1){
							break;
						}
					}

					else{
						//cout << "ARG" << endl;

						break;
					}


				}
			}
			delete &state_copy2;
			if(metodo == 2){
				break;
			}
			s->marked = false;
			i++;

			//

			//delete &state_copy2;

		}

   	tauM->update_factor(mod_factor);
  	list<State*> l=get_next_states(state_actions);



  	//state_actions.clear();


     //siguiente generacion de estados
     return l;
}


} /* namespace clp */
