/*
 * BSGMOP.cpp
 *
 *  Created on: 13 sept. 2017
 *      Author: iaraya
 */

#include "BSGMOP.h"
#include "../../../State.h"

#include <map>


namespace clp {



BSG_MOP::~BSG_MOP() {
	// TODO Auto-generated destructor stub
}




bool BSG_MOP::update(map< pair<double, double>, State*>& NDS, State& state_copy, double valuef1, double valuef2){
    //buscar si domina algun

	map< pair<double, double>, State*>::iterator it;
    for(it=NDS.begin(); it!=NDS.end(); ){
      if((*it).first.first<valuef1 && (*it).first.second<=valuef2)
          it=NDS.erase(it);
      else if((*it).first.first<=valuef1 && (*it).first.second<valuef2)
        it=NDS.erase(it);


      else it++;
    }

    int flag=1;//la bandera se mantiene en cero si no domina a ningun estado
    for(auto NDstate : NDS){
        // nuevo estado es dominado por un estado existente
       if(NDstate.first.first>=valuef1 && NDstate.first.second>=valuef2){
         flag=0; break;
       }
    }

    if(flag==1) {
    	NDS[make_pair(state_copy.get_value(),state_copy.get_value2())] = &state_copy;
    	return true;
    }

    return false;




}


/*
void BSG_MOP::Non_Dominanted_sort(int N,list< pair<State*,State*> >& sorted_list){
  //lista de dominancia por State
  list<int> dominancia(sorted_list.size());
  //iteradores
  list< pair<State*,State*> >::iterator it_sortList;
  list<int>::iterator it_dom;


  //crearemos las fronteras hasta que llegemos al n
  for(int i=0;i<=N){
	  for(it_sortList=sorted_list.begin(), it_dom=dominancia.begin(); it_sortList!=sorted_list.end();){
		  //por elemento revisamos si tiene dominancia
		  //(*it_sortList).second
			for(it_sortList=sorted_list.begin()){

			}

	  }
  }
}*/


//la primera frontera se encuentra formada por todos los estados que no se encuentras dominados, es decir n=0;donde n se obtiene al contar la cantidad de stados que dominan al estados

//la segunda frontera es formada por los elementos restantes(es decir la lista d estados menos lo elementos de la primera frontera), donde al n presente se le resta 1.
//los elementos que la forman  son aquellos que tienen n=0


void BSG_MOP::select_coeff(list<double>& coeff, int n){
   double coeficiente=100.0/(double) n;
   //coeff.push_back(0);

   for(int i=1;i<=n;i++){
	   coeff.push_back(1.0);
     // coeff.push_back((double)(coeficiente*i)/ 100.0);
   }
}

bool mycomp(pair<State*, State*> a, pair<State*, State*> b ){
	return a.second->get_value() <b.second->get_value();
}

void BSG_MOP::filter_crowding_distance(list< pair<State*, State*> >& frontera, list< pair<State*,State*> >& filtered_states, int n1){
	//list< pair<State*, State*> >::iterator anterior,actual,siguiente,it1;

	//long distance[n1];
	//la distancia de cada punto se iguala a 0
	//se ordena la frontera por el las funciones objetivos, como el problema es biobjetivo y son no dominados, solo lo ordenaremos por la primera funcion objetivo
	//

	//sort(frontera.begin(),frontera.end(),mycomp);
	//se debe ordenar la frontera si o si dejando al primer elemento como el que maximiza el objetivo 1(get_value)
	//it1=frontera.begin()++;// el for comienza desde el segundo elemento y termina en el penultimo
	//for(int i=0;it1!=frontera.end()--;i++){

	//	actual=it1;
	//	anterior=actual--;
	//	actual=it1;
	//	siguiente=actual++;
		//valor siguiente - valor anterior/
	//	distance[i]=distance[i]+((siguiente->second->get_value()-anterior->second->get_value())/(frontera.begin()->second->get_value()-frontera.begin()->second->get_value2()));
	//}

}


/** TODO: Eliminar final_state (delete final_state) de pares descartados del mapa states
 *  y eliminar pares descartados del mapa states (fronteras descartadas y soluciones descartadas de ultima frontera)
 */

void BSG_MOP::filter_nondominated_sort (list< pair<State*,State*> >& filtered_states, int n) {
	map< pair<double, double>, pair<State*, State*> >::iterator it1, it2; //it1=states , it2=filtered_states
	list< pair<State*, State*> > frontera;
	list< pair<State*, State*> >::iterator it3;


	while(true){
		frontera.clear();
		for(it1=state_actions.begin();it1!=state_actions.end();){
			State* s= it1->second.first;
			State* final_state= it1->second.second;
			Action* a = (s)? s->next_action(*final_state):NULL;
			if(!a) {
				delete final_state; it1++;
				continue;
			}

			int domin=0;
			for(it2=state_actions.begin();it2!=state_actions.end();it2++){
				if((*it1)!=(*it2))
				if(final_state->get_value()<=final_state->get_value()){
					domin=domin+1;
				}
			}
			if( domin == 0){
				frontera.push_back(it1->second);
				it2=it1;
				it2++;
			    state_actions.erase(it1);
			    it1=it2;
			}else it1++;


		}

		if((filtered_states.size()+frontera.size())<=n){
			for(it3=frontera.begin();it3!=frontera.end();it3++){

				State* s= it3->first;
				State* final_state= it3->second;
				Action* a = (s)? s->next_action(*final_state):NULL;

		 		s=s->copy();
				s->transition(*a);
				it3->first=s;

				filtered_states.push_back((*it3));
			}
			//filtered_states.push_back()
		}
		else{
			filter_crowding_distance(frontera,filtered_states,(n-filtered_states.size()));

			break;
		}
	}



}



list<State*> BSG_MOP::next(list<State*>& S){

    //no hay mas estados en el arbol
    if(S.size()==0) return S;
    int n=5;//aun no se de donde lo obtendremos


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
        	dynamic_cast<MO_ActionEvaluator*>(state.get_evaluator())->set_alpha(alpha);

        	state.get_best_actions(best_actions, w);


        	for(auto action : best_actions)
        		action_alpha.push_back(make_pair(action,alpha));
        }




        //Actions are evaluated using the greedy algorithm
        for(auto a_a : action_alpha){
        	State& state_copy = *state.copy();
        	state_copy.transition(*a_a.first);
        	delete a_a.first;

        	//cout << a_a.second << endl;

        	dynamic_cast<MO_ActionEvaluator*>(state.get_evaluator())->set_alpha(a_a.second);
        	greedy.run(state_copy, timelimit, begin_time);

        	pair<double, double> value = make_pair(state_copy.get_value(), state_copy.get_value2());

        	//si state_copy es solucion no dominada se agrega a NDS
        	bool bbb = update(NDS, *state_copy.copy(), value.first,value.second);
        	if(bbb){
        		cout << "new best solution found: (" << value.first << "," << value.second << ")" << endl;
        		//cout << "NDS size:" << NDS.size() << endl;
        	}

        	//se inserta el estado si no hay uno equivalente en el mapa
        	if(state_actions.find(value) == state_actions.end())
        		state_actions.insert(make_pair(value,  make_pair(&state, &state_copy)) );

        }

    }


    list< pair<State*,State*> >filtered_states;

	filter_nondominated_sort (filtered_states, beams);

	return filtered_states;

	/*
	map< pair<double, double>, pair<State*, State*>, nd_sort > ::iterator state_action=sorted_states.begin();

	//par que marca el final de las soluciones no dominadas
	pair<double, double>  last_pair (make_pair(0.0, 0.0));
	bool first=true;

	//Para cada state->final_state se rescata la accion
	//Si no hay acción posible o si la cuota the beams ha sido sobrepasada
	//se elimina final_state y el elemento del mapa
	int k=0;
	while(state_action!=sorted_states.end()){
		State* s= state_action->second.first;
		State* final_state=state_action->second.second;
		Action* a = (s)? s->next_action(*final_state):NULL;

		if((nextS.size()<beams || state_action->first.first > last_pair.first) && a){
			s=s->copy();
			state_action->second.first=s;
			s->transition(*a);
			nextS.push_back(s);
			if(state_action->first.first > last_pair.first) last_pair = state_action->first;
			else first=false;
	 }else state_action->second.first=NULL;

		//other elements are removed from the state_actions
		 if(k>=beams && !first){
			delete final_state;
		   state_action=sorted_states.erase(state_action);
		 }else state_action++;

	 if(a) delete a;
	 k++;
	}

*/

}

} /* namespace clp */
