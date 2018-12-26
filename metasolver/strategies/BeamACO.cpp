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
        // cout << state.get_value() << endl;

         if(state.is_root()) cout << "beams/max_level_size:" << beams << "/" << max_level_size << endl;


         //se obtiene la lista de las mejores acciones a partir del estado actual
         list< Action* > best_actions;
         
         //each level of the search tree should explore max_level_size nodes, thus...
         w =  (double) max_level_size / (double) S.size() + 0.5;

         get_best_actions_aco(state, best_actions, w);



         //the actions are evaluated and saved in the sorted map
         list< Action* >::iterator it = best_actions.begin();

         for(; it!=best_actions.end()&& get_time()<=timelimit;it++){

        	 State& state_copy = *state.clone();
        	 state_copy.transition(**it);
        	 //cout << state_copy.get_value() << endl;
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
            	// cout << value << endl;
            	 state_actions[-value]= make_pair(&state, &state_copy);
             }else delete &state_copy;

         }
     }

    i=0;
   	for(auto state_action:state_actions){
   		if(i==w) break;
 		State* s= state_action.second.first;
 		State* final_state=state_action.second.second;
 		Action* a = (s)? s->next_action(*final_state):NULL;
 		if(a)
 			//cout << "///////////" << endl;
 			//cout << tauM->get_tau(s,a) << endl;
 			tauM->incr_tau(s,a,0.1);
 			//tauM->update_factor(0.99);
 			//cout << tauM->get_tau(s,a) << endl;
 			//cout << "///////////" << endl;
 			break;
 		i++;
   	}
   	tauM->update_factor(0.99);

  	list<State*> l=get_next_states(state_actions);



  	//state_actions.clear();


     //siguiente generacion de estados
     return l;
}


} /* namespace clp */
