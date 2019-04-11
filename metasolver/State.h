/*
 * State.h
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

#include <map>
#include <string>
#include <list>
#include <iostream>
#include <functional>

#ifndef STATE_H_
#define STATE_H_

#include "ActionEvaluator.h"
#include <algorithm>

using namespace std;
namespace metasolver{

class Action{
public:

	Action() : state_code(0) { };

	virtual Action* clone() const=0;

	virtual ~Action(){}

	//codifica estado previo
	long state_code;
	vector<double> parameter_values;
};

/**
 * Represent a partial solution of the problem. It consists in
 * the container (block) with the located blocks, a map of the left boxes and
 * a list of valid blocks which can be put in the container.
 */
class State {
public:

	State() : parent (NULL), id(count_states++), sim_value(0), state_changed(false){}

	virtual State* clone() const = 0;


	State(const State& S) : parent(&S), id(count_states++), sim_value(S.sim_value), state_changed(false){
		list<const Action*>::iterator it=S.get_path().begin();
		for(;it!=S.path.end();it++)
			path.push_back((*it)->clone());
	}

	virtual State* create_neighbor(State* s0){
		return NULL;
	}

	virtual ~State() {
		while(!path.empty()){
			delete path.front();
			path.pop_front();
		}
	}

	/**
	 * The value of the objective function
	 */
	virtual double get_value() const = 0;

	/**
	 * The value of the second objective function
	 */
	virtual double get_value2() const { return 0.0; }

	void transition(const Action& action) {
		long old = get_code();
		path.push_back(action.clone());
		_transition(action);
		if( get_code() != old ) state_changed=true;
		else state_changed=false;
	}
	
	/*
	* Rearranges the elements in the path randomly
	*/
	virtual int shuffle_path(){
	    cout << "why!!" << endl;
	    return 0;
	   // std::random_shuffle ( path.begin(), path.end() );
	}

	virtual void get_actions(list< Action* >& actions) const = 0;

	virtual Action* next_action(State& final);

	bool is_root(){ return (path.size()==0); }

	list<const Action*>& get_path() const{ return path;}

	virtual void print() {  }

	const list<const State*>& get_children() const { return children;}

	void add_children(State* s){ children.push_back(s); }

	int get_id() const { return id; }

	const State* get_parent() const{ return parent; }

	virtual long get_code() const = 0;

	static int count_states;

	double sim_value;

	bool state_changed;

protected:

	virtual void _transition(const Action& action) = 0;

	const State* parent;
	//bool root;

	list<const State*> children;

	//list of actions for reconstructing the state from scratch
	mutable list<const Action*> path;

	int id;



};


}

#endif /* STATE_H_ */
