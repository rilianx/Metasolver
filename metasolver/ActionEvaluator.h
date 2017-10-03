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

	double get_r() const { return r; }

protected:


	private:
	double r;

};

class MO_ActionEvaluator : public ActionEvaluator{
public:
	MO_ActionEvaluator(ActionEvaluator* ev1=NULL, ActionEvaluator* ev2=NULL, double r=0.0) :
		ev1(ev1), ev2(ev2), ActionEvaluator(r), alpha(1.0) { }

	virtual ~MO_ActionEvaluator(){ }

	virtual double eval_action(const State& s, const Action& a){
		double ev=eval_action1(s,a);
		double evv=eval_action2(s,a);

		if(ev1) ev = ev1->eval_action(s, a);
		if(ev2) evv = ev2->eval_action(s, a);

		return alpha*ev + (1.0-alpha)*evv;
	}

	//set the value of alpha
	virtual void set_alpha(double a){
		alpha = a;
	}


protected:
	//for the first and second objectives
	virtual double eval_action1(const State& s, const Action& a) {return 0.0;}
	virtual double eval_action2(const State& s, const Action& a) {return 0.0;}

private:
	double alpha;
	ActionEvaluator* ev1;
	ActionEvaluator* ev2;

};


}

#endif /* HEURISTIC_FUNCTION_H_ */
