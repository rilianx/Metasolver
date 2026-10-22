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
#include <cstdint>




#ifndef STATE_H_
#define STATE_H_


#include "ActionEvaluator.h"
#include <algorithm>


using namespace std;
namespace metasolver{

class Action{
public:
	virtual Action* clone() const=0;

	virtual ~Action(){}

};



/**
 * Represent a partial solution of the problem. It consists in
 * the container (block) with the located blocks, a map of the left boxes and
 * a list of valid blocks which can be put in the container.
 */
class State {
public:

	State() {}

	virtual State* clone() const = 0;


	State(const State& S) {
		list<const Action*>::iterator it=S.get_path().begin();
		for(;it!=S.path.end();it++)
			path.push_back((*it)->clone());
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

	void transition(const Action& action) {
		path.push_back(action.clone());
		_transition(action);
	}
	
	/**
	 * Retorna (en bactions) las n mejores acciones segun el evaluador, ordenadas
	 * de mejor a peor
	 */
	virtual int get_best_actions(ActionEvaluator* evl, list< Action* >& bactions, int n) const = 0;

	/**
	 * Identificador del estado (0 si no se implementa). Dos estados con el mismo
	 * hash deben ser equivalentes. Se utiliza en Greedy para no repetir calculos.
	 */
	virtual uint64_t get_hash() const { return 0; }

	virtual Action* next_action(State& final);

	bool is_root(){ return (path.size()==0); }

	list<const Action*>& get_path() const{ return path;}

protected:

	virtual void _transition(const Action& action) = 0;

	//list of actions for reconstructing the state from scratch
	mutable list<const Action*> path;
};


}

#endif /* STATE_H_ */
