/*
 * State.h
 *
 *  Created on: 03/19/2019
 *      Author: ignacio
 */

#ifndef mctsNode_H_
#define mctsNode_H_

#include <map>
#include <set>
#include <string>
#include <list>
#include <iostream>
#include <functional>
#include <assert.h>

#include <algorithm>


#include "State.h"

using namespace std;
namespace metasolver{

class mctsNode;

struct  node_comp{
	bool operator()(const mctsNode* n1, const mctsNode* n2);
};

/**
 * A mctsNode in the search tree
 */


class mctsNode {
public:

  mctsNode(mctsNode* parent, const Action* a, map<int, int>& level2nodes, map<int, int>& level2selectednodes,
		  map< int, set<mctsNode*, node_comp> >& level_nodes);

	virtual ~mctsNode() {
		if(action) delete action;
	}



	int rank (){
		int r=1;
		for(auto n:level_nodes){
			if(n==this) return r;
			r++;
		}
		return r;
	}

	int hindex (){
		int h=0;
		for(auto n:level_nodes){
			if(n->nb_simulations>h || n->get_pre_children().empty()) h++;
			else break;
		}
		return h;
	}


	// Generate the state from the root
	virtual State* get_state(const State* s0);

	bool is_root(){ return parent==NULL; }

	list<mctsNode*>& get_children() { return children;}

	list<mctsNode*>& get_pre_children() { return pre_children;}

	void add_pre_children(mctsNode* n){ pre_children.push_back(n); }

	void add_children(mctsNode* n){ children.push_back(n); }

	mctsNode* next_child(){
		if(!pre_children.empty()){
			children.push_back(pre_children.front());
			pre_children.pop_front();
			return children.back();
		}
		return NULL;
	}

	const Action* get_action(){ return action; }

	int get_id() const { return id; }

	const mctsNode* get_parent() const{ return parent; }

	inline void set_promise(double p) {promise=p;}
	inline void set_mean(double m) {mean=m;}

	//GETTERS
	double get_mean() const {return mean;}
	double get_promise() const{return promise;}
	double get_var() const {return var;}
	double get_depth()const{return depth;}
	double get_sym()const{return sym;}


	// var operations
	void increase_depth(){ depth++;}
	void initialize_depth(double val ){depth=val;}

	double calculate_promise();

	virtual void update_simulations(double new_value, int depth_subroot){
		if(depth<depth_subroot && !bp) return;

		nb_simulations++;
		if(nb_simulations<=1){
		    sym = new_value;
		}
		if(best<new_value){
		    best=new_value;
		}

		if(nb_simulations >= 1)
			mean = (mean*(nb_simulations-1)+new_value)/nb_simulations;


    //actualiza la varianza
		if(nb_simulations>=2)
			var = ((var*(nb_simulations-2)) +
			      pow( (new_value-mean),2) ) / ( nb_simulations-1 );

		//calculate_promise();

    //back-propagation
		if(parent)
		   parent->update_simulations(new_value,depth_subroot);
	}

	static int count_nodes;

  bool selectable;
  int selected_count; //number of times the node has been selected

  int nb_simulations;

  static double B; //weight of std.dev.
  static double C; //weight of nb-simulations
  static bool bp; //backpropagation

  //treenode-elements
	mctsNode* parent;
	//list of simulated children
	list<mctsNode*> children;
	//list of non-simulated children
	list<mctsNode*> pre_children;



  //mcts-elements
	double sym; //evaluacion de la primera simulacion
	double best; //mejor simulacion de nodos hijos
	int depth; //profundidad del nodo
    int& N; //cantidad de nodos en el nivel
    int& N_next; //cantidad de nodos en el siguiente nivel
    int& SN; //cantidad de nodos seleccionados en el nivel
    set<mctsNode*, node_comp>& level_nodes;

	double var;
	double mean;
	double promise;

	const Action* action;

	int id;




};




class Compare{
public:
		bool operator() (const mctsNode* a , mctsNode * b){
				if(a->get_promise() > b->get_promise() ) return true;
				else if(a->get_promise() == b->get_promise()){
						if(a->get_promise()  > b->get_promise() ) return true;
						else return false;
				}
				return false;
		}
};


}

#endif /* STATE_H_ */
