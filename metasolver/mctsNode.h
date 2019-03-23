/*
 * State.h
 *
 *  Created on: 03/19/2019
 *      Author: ignacio
 */

#ifndef mctsNode_H_
#define mctsNode_H_

#include <map>
#include <string>
#include <list>
#include <iostream>
#include <functional>
#include <assert.h>

#include <algorithm>


#include "State.h"

using namespace std;
namespace metasolver{


/**
 * A mctsNode in the search tree
 */
class mctsNode {
public:

  mctsNode(mctsNode* N, const Action* a);

	virtual ~mctsNode() {
		if(action) delete action;
	}



	// Generate the state from the root
	virtual State* get_state(const State* s0);

	bool is_root(){ return parent==NULL; }

	const list<mctsNode*>& get_children() const { return children;}

	const list<mctsNode*>& get_pre_children() const { return pre_children;}

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

	double get_mean() const {return mean;}
	double get_promise() const{return promise;}
	double get_var() const {return var;}

	void calculate_promise();

	virtual void update_simulations(double new_value){
		nb_simulations++;

		if(nb_simulations >= 1)
			mean = (mean*(nb_simulations-1)+new_value)/nb_simulations;


    //actualiza la varianza
		if(nb_simulations>=2)
			var = ((var*(nb_simulations-2)) +
			      pow( (new_value-mean),2) ) / ( nb_simulations-1 );

		calculate_promise();

    //back-propagation
		if(parent)
		   parent->update_simulations(new_value);
	}

	static int count_nodes;

	bool selected;

  int nb_simulations;

  static double B; //weight of std.dev.
  static double C; //weight of nb-simulations

protected:

  //treenode-elements
	mctsNode* parent;
	//list of simulated children
	list<mctsNode*> children;
	//list of non-simulated children
	list<mctsNode*> pre_children;



  //mcts-elements
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