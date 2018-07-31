/*
 * A_star.h
 *
 *  Created on: 24 may. 2018
 *      Author: iaraya
 */

#ifndef STRATEGIES_A_STAR_H_
#define STRATEGIES_A_STAR_H_


#include <SearchStrategy.h>
#include <State.h>
#include <queue>
#include <unordered_set>

//TODO: Al seleccionar un nodo se debe aplicar el mecanismo correspondiente para poder calcular promise
//TODO: Implementar funcion State::promise que calcula la probabilidad de que si lanzo una nueva
//simulacion del nodo, la evaluacion sera mayor a la mejor obtenida más un delta pequeño (0.0001).
//Para calcular el promise se necesita saber el promedio de las simulaciones desde el nodo y
//la desviacion estandar. Por lo que se requieren a lo menos 2 evaluaciones.
//Si el nodo tiene solo una evaluacion, le copia la desviacion a su hermano que tenga al menos 2.
namespace metasolver {

    class Compare{
    public:
        bool operator() (State* a , State* b){
            if(a->promise() < b->promise() ) return false;
            else if(a->promise() == b->promise()){
                if(a->promise()  < b->promise()) return false;
                else return true;
            }
            return true;
        }
    };

    class MCTS : public SearchStrategy {
        public:

    	MCTS() {}

	    virtual ~MCTS() {}

	/**
	 * Run the strategy and return the best found value
	 */
	virtual double run(State& s, double tl=99999.9, clock_t bt=clock()){

		std::priority_queue<State* , vector<State*>, Compare> q;
		q.push(s.clone());

		while(q.size() > 0){
			State* s = q.top() ; q.pop();

			//Lanzar el greedy tantas veces como sea necesario para poder calcular promise
			//Generar varios hijos como sea
			/*
			 * Consiste en que cada vez que se escoja un nodo se generen (al menos) dos o tres
			 * simulaciones a partir de ese nodo y de su mejor hijo. De esta manera, la
			 * selección del siguiente nodo se puede realizar directamente con la información
			 * de los nodos (media y desviación) sin necesidad de tener que estimar el valor de
			 * la desviación estándar.

	Para comenzar a escoger nodos usando las probabilidades, se deben
	realizar n simulaciones a partir de cada uno de los n hijos del nodo
	raíz, con n el mínimo de simulaciones necesarias por nodo para poder
	calcular las probabilidades (e.g., n=3).
    En el peor caso, se seleccionará un nodo con n hijos "hoja".
    Se deberán realizar n-1 simulaciones para cada hijo, y n
    simulaciones para el nuevo hijo del nodo, i.e., n^2 simulaciones.
    En el mejor caso se deberán realizar n simulaciones nuevas.
			 */

			cout << s->promise() << endl;

			//Se generan todos los vecinos posibles de la Matriz Actual
			list<Action*> actions;
			s->get_actions(actions);


			for(auto action:actions){
				State* copy=s->clone();
				copy->transition(*action);



               	q.push(copy);
			}

			delete s;
		}

		return best_state->get_value();
	}


};

} /* namespace metasolver */

#endif /* STRATEGIES_A_STAR_H_ */
