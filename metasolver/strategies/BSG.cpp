/*
 * BSGpath.cpp
 *
 *  Created on: 12 jul. 2017
 *      Author: iaraya
 */

#include "BSG.h"

namespace clp {


list<State*> BSG::next(list<State*>& S){
    
     //no hay mas estados en el arbol
     if(S.size()==0) return S;

     //se expanden los nodos de la lista S
     int i=0;
     for(list<State*>::iterator itS=S.begin(); itS!=S.end() && get_time()<=timelimit; itS++,i++){
         State& state=**itS;

         if(state.is_root()) cout << "beams/max_level_size:" << beams << "/" << max_level_size << endl;


         //se obtiene la lista de las mejores acciones a partir del estado actual
         list< Action* > best_actions;
         
         //each level of the search tree should explore max_level_size nodes, thus...
         int w =  (double) max_level_size / (double) S.size() + 0.5;
         state.get_best_actions(best_actions, w);



         //the actions are evaluated and saved in the sorted map
         list< Action* >::iterator it = best_actions.begin();

         for(; it!=best_actions.end()&& get_time()<=timelimit;it++){

        	 State& state_copy = *state.copy();
        	 state_copy.transition(**it);
        	 delete *it;

             double value = greedy.run(state_copy, timelimit, begin_time);


            //best_state update
             if(value > get_best_value()){
            	 if(best_state) delete best_state; 
            	 best_state = state_copy.copy();
            	 cout << "[BSG_path] new best_solution_found ("<< get_time() <<"): " << value << " "
            			 << best_state->get_path().size() << " nodes" << endl;
             }


             if(state_actions.find(-value)==state_actions.end())
            	 state_actions[-value]= make_pair(&state, &state_copy);
             else delete &state_copy;

         }
     }
     
  
     //siguiente generacion de estados
     list<State*> nextS;
     map<double, pair<State*, State*> >::iterator state_action=state_actions.begin();
     
     //Para cada state->final_state se rescata la accion
     //Si no hay acción posible o si la cuota the beams ha sido sobrepasada
     //se elimina final_state y el elemento del mapa
     int k=0;
     while(state_action!=state_actions.end()){
         
         State* s= state_action->second.first;
         State* final_state=state_action->second.second;
         Action* a = (s)? s->next_action(*final_state):NULL;

         if(nextS.size()<beams && a){
    	    s=s->copy();
    	    state_action->second.first=s;
    	    s->transition(*a);
    	    nextS.push_back(s);
    	 }else state_action->second.first=NULL;
    	   
         //other elements are removed from the state_actions
     	 if(k>=beams){
     	    delete final_state;
            state_action=state_actions.erase(state_action);
     	 }else state_action++;
    	 
    	 if(a) delete a;
    	 k++;
     }

     return nextS;
}


} /* namespace clp */
