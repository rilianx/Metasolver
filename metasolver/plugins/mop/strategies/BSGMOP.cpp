/*
 * BSGMOP.cpp
 *
 *  Created on: 13 sept. 2017
 *      Author: iaraya
 */

#include "BSGMOP.h"
#include <unordered_map>

namespace clp {



BSG_MOP::~BSG_MOP() {
	// TODO Auto-generated destructor stub
}




void BSG_MOP::update(list<State*>& NDS, State& state_copy, double valuef1, double valuef2){
    //buscar si domina algun

    list<State*>::iterator it;
    for(it=NDS.begin(); it!=NDS.end(); ){
      if((*it)->get_value()<=valuef1 && (*it)->get_value2()<=valuef2)
        it=NDS.erase(it);
      else it++;
    }

    int flag=1;//la bandera se mantiene en cero si no domina a ningun estado
    for(auto NDstate : NDS){
        // nuevo estado es dominado por un estado existente
    	if(NDstate->get_value()>=valuef1 && NDstate->get_value2()>=valuef2){
        flag=0; break;
      }
    }

    if(flag==1)NDS.push_back(&state_copy);



}

void BSG_MOP::select_coeff(list<double>& coeff, int n){
   double coeficiente=100.0/(double) n;
   coeff.push_back(0);

   for(int i=1;i<=n;i++){
     coeff.push_back((double)(coeficiente*i)/ 100.0);
   }
}

list<State*> BSG_MOP::next(list<State*>& S){

    //no hay mas estados en el arbol
    if(S.size()==0) return S;
    int n=10;//aun no se de donde lo obtendremos

    //se expanden los nodos de la lista S
    int i=0;
    for(list<State*>::iterator itS=S.begin(); itS!=S.end() && get_time()<=timelimit; itS++,i++){
        State& state=**itS;

        //if(state.is_root()) cout << "beams/max_level_size:" << beams << "/" << max_level_size << endl;

        //se genera la lista de coeficientes que ponderan las funciones objetivo
        //max alpha*f1 + (1-alpha)*f2
        list<double> alpha_v;
        BSG_MOP::select_coeff(alpha_v, n);

        list < pair < Action*, double > > action_alpha;

        //se obtienen las mejores acciones para cada valor de alpha
        for(auto alpha : alpha_v){

            //each level of the search tree should explore max_level_size nodes, thus...
          int w =  (double) max_level_size / (double) S.size() + 0.5;

        	list< Action* > best_actions;
        	evl.set_alpha(alpha);
        	state.get_best_actions(best_actions, w);

        	for(auto action : best_actions)
        		action_alpha.push_back(make_pair(action,alpha));
        }

        //nd_sort ordena por dominancia
        map< pair<double, double>, pair<State*, State*>, nd_sort > sorted_states;

        //Actions are evaluated using the greedy algorithm
        for(auto a_a : action_alpha){
        	State& state_copy = *state.copy();
        	state_copy.transition(*a_a.first);
        	delete a_a.first;

        	evl.set_alpha(a_a.second);
        	greedy.run(state_copy, timelimit, begin_time);

        	pair<double, double> value = make_pair(state_copy.get_value(), state_copy.get_value2());

        	//si state_copy es solucion no dominada se agrega a NDS
        	update(NDS, *state_copy.copy(), value.first,value.second);




            if(sorted_states.find(value)==sorted_states.end())
            	sorted_states[value]= make_pair(&state, &state_copy);
            else delete &state_copy;


        }

        //TODO: ordenar por distance crowding

        //siguiente generacion de estados

        return get_next_states(sorted_states);

    }
}

} /* namespace clp */
