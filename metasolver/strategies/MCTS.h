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
        bool operator() (const State* a , const State* b){
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

    	MCTS(ActionEvaluator* evl, SearchStrategy& greedy) : SearchStrategy(evl), greedy(greedy) {}

	    virtual ~MCTS() {}

	/**
	 * Run the strategy and return the best found value
	 */
	virtual double run(State& s, double tl=99999.9, clock_t bt=clock()){

		std::priority_queue<const State* , vector<const State*>, Compare> q;
		q.push(s.clone());

		while(q.size() > 0){
			const State* s = q.top();
			cout << s->promise() << endl;

			const State* s2 = simulate(s);

			if(s2 == NULL){
				q.pop();
				delete s;
			}else if(s->get_children().size() == 3){
				for(auto ch : s->get_children()){
					simulate(ch);
					q.push(ch);
				}
			}else if(s->get_children().size() > 3){
				simulate(s2);
				q.push(s2);
			}

		}

		return best_state->get_value();
	}

    // performs a simulation and returns the corresponding child
    State* simulate(const State* s){

    	return NULL;
    }

    private:

	SearchStrategy& greedy;


};



} /* namespace metasolver */

#endif /* STRATEGIES_A_STAR_H_ */
