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
#include <fstream>

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
            if(a->get_promise() > b->get_promise() ) return false;
            else if(a->get_promise() == b->get_promise()){
                if(a->get_promise()  > b->get_promise() ) return false;
                else return true;
            }
            return true;
        }
    };

    class MCTS : public SearchStrategy {
        public:

    	MCTS(ActionEvaluator* evl, SearchStrategy& greedy) : SearchStrategy(evl), greedy(greedy) {}

	    virtual ~MCTS() {}


	    void dfsPrintChild(const State* node, ofstream& file){
	    	file << "{ "<<endl;
	    	file<<"\t \"name\":\""<<node->get_id()<<"\",";
	    	file<<"\t \"parent\":\""<<node->get_parent()->get_id() <<"\",";
	    	file<<"\t \"value\":\""<<node->get_value() <<"\",";
	    	file<<"\t \"sd\":\"\",";
	    	file<<"\t \"mcts_value\":\"\",";
	    	file<<"\t \"stimated_sd\":\"\",";
	    	file<<"\t \"ponderated_sd\":\"\",";
	    	file<<"\t \"depth\":\"\",";
	    	file<<"\t \"num_visit\":\"\"";
	    	file<< "\t,\"simulations\":[]"<<endl;


	    	if(!node->get_children().empty()){
	    		file<< "\t,\"children\":["<<endl;
	    		for(auto c:node->get_children()){
	    			dfsPrintChild(c,file);
	    			if(c!=node->get_children().back())
	    			    file<<","<<endl;
	    		}
	    		file<<"]";
	    	}
	    	file<<"}"<<endl;
	    }

	    void pointsToTxt(State* root, int it) {
	    	ofstream myfile("problems/clp/tree_plot/flare"+std::to_string(it)+".json");
	    	dfsPrintChild(root,myfile);
	    }

	/**
	 * Run the strategy and return the best found value
	 */
	 double run(State& s, double tl=99999.9, clock_t bt=clock()){

		std::priority_queue<const State* , vector<const State*>, Compare> q;
		q.push(s.clone());



		while(q.size() > 0){
			bool change_best=false;

			const State* s = q.top(); q.pop();
			cout << s->get_promise() << endl;

			const State* s2 = simulate(s, change_best);

			if(s2 == NULL){
				q.pop();
				delete s;
			}else if(s->get_children().size() == 3){
				for(auto ch : s->get_children()){
					simulate(ch, change_best);
					simulate(ch, change_best);
					q.push(ch);
				}
			}else if(s->get_children().size() > 3){
				simulate(s2, change_best);
				simulate(s2, change_best);
				q.push(s2);
			}

			q.push(s);

			if(change_best) update_queue(q);

		}

		pointsToTxt(&s, 0);
		system("firefox problems/clp/tree_plot/index.html");

		return best_state->get_value();
	}

	// update to promise list
	void update_queue(std::priority_queue<const State* , vector<const State*>, Compare>& q)
	{
		std::priority_queue<const State* , vector<const State*>, Compare> aux;
		while(!q.empty()){
			aux.push(q.top());
			q.pop();
		}
		q=aux;
	}

    // performs a simulation and returns the corresponding child
    State* simulate(const State* s, bool change_best){
    	double value;
    	int size = s->get_children().size();
		//TODO: ELIMINAR COUT
		cout << "[MCTS] {simulate} estado: ("<< s->get_id()<<") mean: ("<< s->get_mean() << ")var: ("<< s->get_var() 
		<< ") promise:  ("<< s->get_promise()<<")";
    	//TODO: optimizar!
   		State* s2=s->clone();
   		list< Action* > best_actions;
   		get_best_actions(*s2, best_actions, size+1);
   		s2->transition(*best_actions.back());
   		State* s3=s2->clone();
   		value=greedy.run(*s3);


        //best_state update
        if(value > get_best_value()){
          if(best_state) delete best_state;
          best_state = s3->clone();
          change_best=true;
          cout << "[MCTS] new best_solution_found ("<< get_time() <<"): " << value << " "
        		 << best_state->get_path().size() << " nodes" << endl;
        }	
		
        s->add_children(s2);
        s->update_values(value);
		s->calculate_promise(get_best_value());

        return s2;
    }



    private:

	SearchStrategy& greedy;


};



} /* namespace metasolver */

#endif /* STRATEGIES_A_STAR_H_ */
