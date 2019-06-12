/*
 * MCTS.cpp
 *
 *  Created on: Mar 19, 2019
 *      Author: iaraya
 */

#include "MCTS.h"
#include "mctsNode.h"



namespace metasolver {

	bool MCTS::discard_equivalent=false;

	void MCTS::dfsPrintChild(mctsNode* node, ofstream& file){

		file << "{ "<<endl;
		file<<"\t \"name\":\""<<node->get_id()<<"\",";
		file<<"\t \"parent\":\""<< ((node->get_parent())? node->get_parent()->get_id():-1) <<"\",";
		file<<"\t \"mean\":\""<<node->get_mean() << "\",";
		file<<"\t \"sd\":\""<<node->get_var() <<"\",";
		file<<"\t \"mcts_value\":\""<<node->get_promise() <<"\",";
		file<<"\t \"children_size\":\""<<node->get_children().size() <<"\",";
		file<<"\t \"simulations\":\""<<node->nb_simulations <<"\",";
		file<<"\t \"ponderated_sd\":\"\",";
		file<<"\t \"depth\":\"\",";
		file<<"\t \"num_visit\":\"\"";



		if(!node->get_children().empty()){
			file<< "\t,\"children\":["<<endl;

			for(auto c:node->get_children()){
				if(c->selectable==true){
				  dfsPrintChild(c,file);
				  if(c!=node->get_children().back())
					  file<<","<<endl;
				}
			}
			file<<"]";
		}
		file<<"}"<<endl;
	}

    void MCTS::pointsToTxt(mctsNode* root, int it) {
    	ofstream myfile("problems/clp/tree_plot/flare"+std::to_string(it)+".json");
    	dfsPrintChild(root,myfile);
    }



    	//TODO:: optimizar esta funcion
		mctsNode* MCTS::select_node(){

			double max_promise=0.0;
			mctsNode* best=NULL;
			for(auto node:nodes){
				if(node->get_pre_children().empty()) continue;

				node->calculate_promise();
				if(node->get_promise()>max_promise ){
					max_promise=node->get_promise();
					best=node;
				}
			}

			if(best){
				if(best->selected_count==0){
				   //level2nodes[best->get_depth()+1]+=best->get_children().size();
				   level2selectednodes[best->get_depth()]++; //incrementa contador del mapa de selected
				}
				best->selected_count++;
			}

			return best;

			/*
			mctsNode* c=NULL;
			do {
				c= *nodes.begin();
				nodes.erase(nodes.begin());
			} while(c->get_pre_children().empty() && !nodes.empty());
			return c;
			*/

		}

    double MCTS::run(State& s, double tl, clock_t bt){
    		timelimit=tl;
			cout << "clone state" << endl;
    		s0 = s.clone();
            typedef pair<int, int> level;

			cout << "root creation" << endl;
    		mctsNode* root=new mctsNode(NULL,NULL,level2nodes,level2selectednodes,level_nodes);

    		cout<< "initializing depth and sym"<< endl;
    		root->initialize_depth(0);


    		cout << "depth: "<< root->get_depth()<< " sym: " <<root->get_sym()<< endl;

    		//Se realizan dos simulaciones del nodo raiz y se aumenta el contador sym
    		cout << "Se realizan la simulacion del nodo raiz" << endl;

    		make_selectable(root,s0);
    		simulate(root,s0);
    		//simulate(root,s0);

			int i=0;
    		while(nodes.size() > 0 &&  get_time()<timelimit){
				i++;
    			//Seleccion del nodo
    			mctsNode* n = select_node();
    			//if(greedy.runs>1000) break;
    			//if(n->hindex()==5) break;

    			if(n==NULL) break;
				//se simula n
				simulate(n,s0);

				//b metasolver/strategies/MCTS.cpp:123
				const list<mctsNode*>& children = n->get_children();

				//los hijos del nodo se vuelven seleccionables
				if(children.size() >= 2){
				  for(auto ch : children){
					if(!ch->selectable){
						if(discard_equivalent && level_nodes[ch->get_depth()].find(ch) != level_nodes[ch->get_depth()].end()){
							ch->selectable=true; //en realidad habria que eliminar el nodo
							level2nodes[ch->get_depth()]++;
							continue;
						}

						if(discard_equivalent) level_nodes[ch->get_depth()].insert(ch);
						make_selectable(ch,s0);
						//simulate(ch,s0);

					}
				}
    			}
    		}

    		 	 cout << "selectable nodes: ";
				for(auto nod:level2nodes)
				  cout << nod.first << ":" << nod.second  << " ";
				cout <<endl;

				cout << "selected nodes: ";
				for(auto nod:level2selectednodes)
				  cout << nod.first << ":" << nod.second  << " ";
				cout <<endl;

    		//pointsToTxt(root, 0);
    		//system("firefox problems/clp/tree_plot/index.html");
    		return best_state->get_value();
  }

    void MCTS::make_selectable(mctsNode* n, const State* s0){
    	//se genera el estado
    	State* snext=n->get_state(s0);

		list< Action* > best_actions;
		get_best_actions(*snext, best_actions, 999);
		for(auto a:best_actions){
			if(n->get_children().empty() || (*a!=*n->get_children().front()->get_action())){
				mctsNode* nn=new mctsNode(n,a,level2nodes,level2selectednodes,level_nodes); //agregar nivel al nodo

				n->add_pre_children(nn);
			}
			delete a;
		}
		n->selectable=true;
		level2nodes[n->get_depth()]++;
		nodes.push_back(n);
    }

	void MCTS::simulate(mctsNode* n, const State* s0){
		//no hay hijos no simulados
		if(n->get_pre_children().empty()) return;

		//se genera el estado a partir de la raiz
		State* snext=n->get_state(s0);

		//Se obtiene el siguiente hijo de n y se genera su estado
		mctsNode* next=n->next_child();

		snext->transition(*next->get_action());

        //se lanza un greedy (simulacion)
		double value=greedy.run(*snext);

		//cout << n->get_depth()+1 << ":" << value <<endl;
		//se descartan evaluaciones equivalentes
		/*if(discard_equivalent && level_evals[next->get_depth()].find(value) != level_evals[next->get_depth()].end()){
			n->get_children().pop_back();
			level2nodes[next->get_depth()]++;
			delete snext;
			return;
		}*/



		//if(n->selected_count>0)
			//level2nodes[next->get_depth()]++; //incrementa contador del mapa de nodos por nivel

		//se reconstruye el camino con nodos
		list<const Action*>& actions = snext->get_path();
		bool flag=false;
		for(auto a: actions){
			if(flag){
				next->add_children(new mctsNode(next, a, level2nodes, level2selectednodes, level_nodes));
				next=next->get_children().front();
			}
			if(*a==*next->get_action()) flag=true;
		}

		//se actualiza el valor en el nodo y en todos sus ancentros
		next->update_simulations(value,n->get_depth());

		//best_state update
		if(value > get_best_value()){
		  if(best_state) delete best_state;
		  best_state = snext->clone();
		  cout << "[MCTS] new best_solution_found ("<< get_time() <<"): " << value << " "
								 << best_state->get_path().size() << " nodes" << endl;
		}

		delete snext;
	}
}
