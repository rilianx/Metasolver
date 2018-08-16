/*
 * cpmpEvaluator.h
 *
 *  Created on: 16 ago. 2018
 *      Author: iaraya
 */

#include "ActionEvaluator.h"
#include "cpmpState.h"

#ifndef CPMPEVALUATOR_H_
#define CPMPEVALUATOR_H_

using namespace metasolver;

namespace cpmp {

class cpmpEvaluator : public ActionEvaluator {
public:
	cpmpEvaluator() : ActionEvaluator(0.0)  { };
	virtual ~cpmpEvaluator() { };

	virtual double eval_action(const State& s, const Action& a){
		const cpmpAction* action =dynamic_cast<const cpmpAction*>(&a);
		const cpmpState* state =dynamic_cast<const cpmpState*>(&s);
		return 1.0;
	}
};

}

#endif /* CPMPEVALUATOR_H_ */
