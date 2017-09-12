/*
 * Expander.h
 *
 *  Created on: 5 jul. 2017
 *      Author: iaraya
 */

#include <list>

#include "../metasolver/State.h"

#ifndef EXPANDER_H_
#define EXPANDER_H_

using namespace std;

namespace clp {

/**
 * This class is responsible of identifying the best actions for each state
 */
class Expander {
public:
	Expander() { };
	virtual ~Expander() { };

	virtual Action* best_action(State& s){
		list< Action* > actions;
		if(best_actions(s,1,actions)) return *actions.begin();
		else{
			//cout << "NULL" << endl;
	        return NULL;
		}
	}

	virtual int best_actions(State& s, int n, list< Action* >& actions){
		s.get_actions(actions, n);
		return actions.size();
	}

};

/**
 * This subclass uses an ActionEvaluator for evaluating and selecting the best actions
 */
class ExpanderHF : public Expander {
public:
	ExpanderHF(ActionEvaluator& evl): evl(evl) { };
	virtual ~ExpanderHF(){ };

	virtual int best_actions(State& s, int n, list< Action* >& );

protected:
	ActionEvaluator& evl;

};



} /* namespace clp */

#endif /* EXPANDER_H_ */
