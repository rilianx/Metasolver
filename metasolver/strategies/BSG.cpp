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
     //evl->update_level_alpha(mean,stdev);
     int i=0;
     for(list<State*>::iterator itS=S.begin(); itS!=S.end() && get_time()<=timelimit; itS++,i++){
         State& state=**itS;
        // cout << state.get_value() << endl;

         if(state.is_root()){
        	 double cont = 0;
        	 int aux = alphas.size();
        	 while (alphas.size()!=0){
        		 cont += alphas.back();
        		 alphas.pop_back();
        	 }
        	 gbcont += cont;
        	 gbaux += aux;
        	 if(cont != 0){
        		 mean = gbcont / gbaux;
        		 //mean = cont / aux;
        		 stdev = stdev / sqrt(aux);
        	 }
        	 //double best = alphas.back();
        	 cout << mean << " and " << stdev << endl;
        	 cout << "beams/max_level_size:" << beams << "/" << max_level_size << endl;
         }


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
        	 //cout << state_copy.get_value() << endl;
        	 delete *it;


        	 //cout << state_copy.get_path().size() << endl;
        	 evl->update_parameters(state_copy,mean,stdev);
        	 //cout << evl->get_alpha() << endl;
        	 //cout << "WOW" << endl;
             double value = greedy.run(state_copy, timelimit, begin_time);


            //best_state update
             if(value > get_best_value()){
            	 if(best_state) delete best_state; 
            	 best_state = state_copy.clone();
            	 //cout << evl->get_alpha() << endl;
            	 alphas.push_back(evl->get_alpha());
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
  	//cout << "AYAYA" << endl;
     return l;
}


} /* namespace clp */
