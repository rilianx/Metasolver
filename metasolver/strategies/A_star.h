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

namespace metasolver {

    class Compare{
    public:
        bool operator() (State* a , State* b){
            if(a->get_lower_bound() + a->get_value() < b->get_lower_bound() + b->get_value()) return false;
            else if(a->get_lower_bound() + a->get_value() == b->get_lower_bound() + b->get_value()){
                if(a->get_lower_bound() + a->get_value() < b->get_lower_bound() + b->get_value()) return false;
                else return true;
            }
            return true;
        }
    };

    class A_star : public SearchStrategy {
        public:
	        A_star();
	    virtual
            ~A_star();

	/**
	 * Run the strategy and return the best found value
	 */
	virtual double run(State& s, double tl=99999.9, clock_t bt=clock()){

		std::priority_queue<State* , vector<State*>, Compare> q;
		q.push(s.clone());

		while(q.size() > 0){
			State* s = q.top() ; q.pop();

			cout << s->get_lower_bound() << endl;
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
				//copy->print();
				//exit(0);
				//TODO: revisar si el nuevo estado ya fue creado anteriormente
                if(!visited(copy))
                	q.push(copy);
			}

			delete s;
		}

		return best_state->get_value();
	}

	bool visited(State* s) { return (visited.find(copy) != visited.end()); }

	std::unordered_set<State*> visited;

};

} /* namespace metasolver */

#endif /* STRATEGIES_A_STAR_H_ */
