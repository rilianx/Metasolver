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

namespace metasolver{

class State;
class Action;
/**
 * This class allows to evaluate blocks and spaces in order to
 * select the next action
 */
class ActionEvaluator {
public:
	ActionEvaluator(double r) : r(r), lambda2(0.0)  { };

	virtual ~ActionEvaluator() { }

	virtual double eval_action_rand(const State& s, const Action& a){

		double eval=eval_action(s, a);
		double ran=(r>0.0)? ((double) rand() / (double) RAND_MAX) : 1.0;
		return ( eval + r*log(ran) );
	}

	virtual double eval_action(const State& s, const Action& a) =0;

	//set the weitght of the second objective
	virtual void set_lambda2(double l){
		lambda2 = l;
	}

	virtual double get_lambda2(){
		return lambda2;
	}

	virtual void update_parameters(const State& s) =0;

	double get_r() const { return r; }

protected:

  //weitght of the second objective (lambda1=1-lambda2)
  double lambda2;

	private:
	double r;


};



}

#endif /* HEURISTIC_FUNCTION_H_ */
