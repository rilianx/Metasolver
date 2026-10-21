/*
 * Greedy.h
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

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
	virtual ~ActionEvaluator() { }

	virtual double eval_action(const State& s, const Action& a) =0;
};



}

#endif /* HEURISTIC_FUNCTION_H_ */
