/*
 * Greedy.h
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

#include <cstdlib>
#include <math.h>
#include "State.h"

#ifndef HEURISTIC_FUNCTION_H_
#define HEURISTIC_FUNCTION_H_

using namespace std;

namespace clp{

class State;
class Action;
/**
 * This class allows to evaluate blocks and spaces in order to
 * select the next action
 */
class ActionEvaluator {
public:
	ActionEvaluator(double r) : r(r) { };

	virtual ~ActionEvaluator() { }

	virtual double eval_action_rand(const State& s, const Action& a){
		double eval=eval_action(s, a);
		double ran=(r>0.0)? ((double) rand() / (double) RAND_MAX) : 1.0;
		return (eval * pow(ran, r));
	}

	virtual double eval_action(const State& s, const Action& a) =0;

	private:
	double r;

};

class MO_ActionEvaluator : public ActionEvaluator{
public:
	MO_ActionEvaluator(double r) : ActionEvaluator (r), alpha (1.0) { }

	virtual ~MO_ActionEvaluator(){ }

	//set the value of alpha
	virtual void set_alpha(double a){
		alpha = a;
	}

protected:
	double alpha;

};


}

#endif /* HEURISTIC_FUNCTION_H_ */
