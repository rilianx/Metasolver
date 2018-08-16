/*
 * cpmpEvaluator.h
 *
 *  Created on: 16 ago. 2018
 *      Author: iaraya
 */

#include "ActionEvaluator.h"

#ifndef CPMPEVALUATOR_H_
#define CPMPEVALUATOR_H_

using namespace metasolver;

namespace cpmp {

class cpmpEvaluator : public ActionEvaluator {
public:
	cpmpEvaluator() : ActionEvaluator(0.0)  { };
	virtual ~cpmpEvaluator() { };

	virtual double eval_action(const State& s, const Action& a){
		return 1.0;
	}
};

}

#endif /* CPMPEVALUATOR_H_ */
