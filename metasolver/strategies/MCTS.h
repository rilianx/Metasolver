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
#include <set>
#include <fstream>


namespace metasolver {

    class Compare{
    public:
        bool operator() (const State* a , const State* b){
            if(a->get_promise() > b->get_promise() ) return true;
            else if(a->get_promise() == b->get_promise()){
                if(a->get_promise()  > b->get_promise() ) return true;
                else return false;
            }
            return false;
        }
    };

    class MCTS : public SearchStrategy {
        public:

    	MCTS(ActionEvaluator* evl, SearchStrategy& greedy, double eps, int max_nodes=100, bool bp=false) :
    		SearchStrategy(evl), greedy(greedy), eps(eps), nb_nodes(0), max_nodes(max_nodes), bp(bp) {}

	    virtual ~MCTS() {}


	    void dfsPrintChild(const State* node, ofstream& file){
	    	file << "{ "<<endl;
	    	file<<"\t \"name\":\""<<node->get_id()<<"\",";
	    	file<<"\t \"parent\":\""<<node->get_parent()->get_id() <<"\",";
	    	file<<"\t \"mean\":\""<<node->get_mean() <<"("<< node->get_value() <<")\",";
	    	file<<"\t \"sd\":\""<<node->get_var() <<"\",";
	    	file<<"\t \"mcts_value\":\""<<node->get_promise() <<"\",";
	    	file<<"\t \"children_size\":\""<<node->get_children_size() <<"\",";
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
		timelimit=tl;
		//std::priority_queue<const State* , vector<const State*>, Compare> q;

		std::multiset<const State* , Compare> states;

		bool change_best=false;
		State* s0 = s.clone();
		simulate(s0, change_best);
		simulate(s0, change_best);

		states.insert(s0);

		int i=0;
		while(states.size() > 0 &&  get_time()<timelimit){
			change_best=false;

			const State* s = *states.begin();
			states.erase(states.begin());
			//states.erase(s);

			//cout << s->get_promise() << "," << s->get_children_size() << "," <<states.size() << endl;

			const State* s2 = simulate(s, change_best);
			if(s2 == NULL){
				if(s->get_promise() == 0){
					states.insert(s); //será descartado por recolector de basura (update_queue)
					update_queue(states);
					continue;
				}
			}

			if(s->get_children_size() == 3){
				auto children = s->get_children();

				for(auto ch : children){
					simulate(ch, change_best);
					simulate(ch, change_best);
					if(ch->get_children_size() >= 2){
						ch->calculate_promise(get_best_value()+eps);
						states.insert(ch);
					}else{
						s->remove_children(ch); nb_nodes--;
						nb_nodes-=ch->remove_children();
						delete ch;
					}
				}
			}else if(s2 && s->get_children_size() > 3){
				simulate(s2, change_best);
				simulate(s2, change_best);
				if(s2->get_children_size() >= 2){
					s2->calculate_promise(get_best_value()+eps);
					states.insert(s2);
				}else{
					s->remove_children(s2); nb_nodes--;
					nb_nodes -= s2->remove_children();
					delete s2;
				}
			}

			if(s->get_children_size() >= 2){
				s->calculate_promise(get_best_value()+eps);
				//para dejarlo ordenado necesito sacarlo y colocarlo nuevamente al conjunto
			}

			states.insert(s);


			if(change_best || i%max_nodes==max_nodes-1) update_queue(states);


			i++;
		}
		cout << states.size() << endl;

		//pointsToTxt(s0, 0);
		//system("firefox problems/clp/tree_plot/index.html");

		return best_state->get_value();
	}

	 double nb_nodes;

	// update to promise list
	void update_queue(std::multiset<const State* , Compare>& q)
	{
		std::multiset<const State* , Compare> aux;

		//discard some nodes of the tree (limita uso de memoria)
		while(!q.empty() && (q.size() > max_nodes || (*q.rbegin())->get_promise()==0.0)){

			const State* s = *q.rbegin();
			list<const State*> children = s->get_children();
			for(auto ch:children){
				if(ch->get_children_size()==0){
					s->remove_children(ch);
					nb_nodes--;
					delete ch;
				}else
					ch->remove_parent ();
			}

			if (!q.empty()) {
				std::multiset<const State* , Compare>::iterator it = q.end();
			    --it;
			    q.erase(it);
			}

			if(s->get_parent())
				s->get_parent()->remove_children(s);

			nb_nodes--;
			delete s;

		}

		//cout << q.size() << " -- " << nb_nodes << endl;


		while(!q.empty()){
			//if((*q.begin())->get_children_size() >= 2)
				(*q.begin())->calculate_promise(get_best_value()+eps);

			aux.insert(*q.begin());
			q.erase(q.begin());
		}
		q=aux;

	}



    // performs a simulation and returns the corresponding child
    State* simulate(const State* s, bool& change_best){

    	double value;
    	State* s2, *s3;

        while(true){
			int size = s->get_children_size();

			//TODO: optimizar!
			list< Action* > best_actions;
			get_best_actions(*s, best_actions, size+1);
			if(best_actions.size()<size+1){
				s->set_promise(0.0);
				return NULL;
			}

			s2=s->clone();
			s2->transition(*best_actions.back());
			s3=s2->clone();
			value=greedy.run(*s3);

			//while(evals.size()>100) evals.erase(evals.begin());

			if(evals.find( make_pair(value, s3->get_value2()))==evals.end()){
				evals.insert( make_pair(value,s3->get_value2()) );
				break;
			}else{
				//descarte por similitud (similar a una de las ultimas N soluciones)
				delete s2;
				delete s3;
				s->update_values(value, bp);
				return NULL;
			}
        }



        //best_state update
        if(value > get_best_value()){
          if(best_state) delete best_state;
          best_state = s3->clone();
          cout << "[MCTS] new best_solution_found ("<< get_time() <<"): " << value << " "
                      			 << best_state->get_path().size() << " nodes" << endl;
          change_best=true;
        }
        delete s3;

        s2->set_mean(value);
        s->add_children(s2);
        //cout << "add:" << s2 << endl;
        s->update_values(value, bp);

        nb_nodes++;
        return s2;
    }



    private:

	SearchStrategy& greedy;

	set< pair<double,double> > evals;

	double eps;

	int max_nodes;

	bool bp;

};



} /* namespace metasolver */

#endif /* STRATEGIES_A_STAR_H_ */
