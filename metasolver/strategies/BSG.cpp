/*
 * BSGpath.cpp
 *
 *  Created on: 12 jul. 2017
 *      Author: iaraya
 */

#include "BSG.h"

namespace metasolver {



BSG::~BSG(){

}

list<State*> BSG::next(list<State*>& S){
    
	// cout << "next" << endl;
     //no hay mas estados en el arbol
     if(S.size()==0) return S;

     //se expanden los nodos de la lista S
     int i=0;
     for(list<State*>::iterator itS=S.begin(); itS!=S.end() && get_time()<=timelimit; itS++,i++){
         State& state=**itS;
        // cout << state.get_value() << endl;

         if(state.is_root()) cout << "beams/max_level_size:" << beams << "/" << max_level_size << endl;


         //se obtiene la lista de las mejores acciones a partir del estado actual
         list< Action* > best_actions;
         
         //each level of the search tree should explore max_level_size nodes, thus...
         int w =  (double) max_level_size / (double) S.size() + 0.5;

         get_best_actions(state, best_actions, w);



         //the actions are evaluated and saved in the sorted map
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
            	 cout << "[BSG_path] new best_solution_found ("<< get_time() <<"): " << value << " "
            			 << best_state->get_path().size() << " nodes" << endl;
             }


             if(state_actions.find(-value)==state_actions.end()){
            	// cout << value << endl;
            	 state_actions[-value]= make_pair(&state, &state_copy);
             }else delete &state_copy;

         }
     }


  	// if(global::TRACE){

  		/* cout << "pre_filtered" << endl;
  		 for(auto states:state_actions)
  			 cout << states.first<< endl;

  		 cout << endl;*/
  	 //}


  	list<State*> l=get_next_states(state_actions);
  	//state_actions.clear();


     //siguiente generacion de estados
     return l;
}


} /* namespace clp */
