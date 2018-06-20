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

namespace metasolver {

    class Compare{
    public:
        bool operator() (State a , State b){
            if(a.get_value2() < b.get_value2()) return false;
            else if(a.get_value2() == b.get_value2()){
                if(a.get_value() < b.get_value()) return false;
                else return true;
            }
        }
    };

    class A_star : SearchStrategy {
        public:
	        A_star();
	    virtual
            ~A_star();

	/**
	 * Run the strategy and return the best found value
	 */
	virtual double run(State& s, double tl=99999.9, clock_t bt=clock()){

		//s.get_lower_bound();
		list<Action*> actions;
		s.get_actions(actions);

		State* copy=s.clone();
		for(auto action:actions){
			s.transition(*action);
			delete action;

		}

		std::priority_queue<State* , Compare> q;



		while(q.size() > 0){
			State* s = q.front() ; q.pop();

			if(s->get_lower_bound() == 0){
				best_state = s->clone();
				break;
			}
			//Se generan todos los vecinos posibles de la Matriz Actual
			list<Action*> actions;
			s->get_actions(actions);


			for(auto action:actions){
				State* copy=s->clone();
				copy->transition(*action);
				//TODO: revisar si el nuevo estado ya fue creado anteriormente

				q.push(copy);
			}

			delete s;
		}


	}

};

} /* namespace metasolver */

#endif /* STRATEGIES_A_STAR_H_ */
