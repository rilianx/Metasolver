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

		int containerA = state->topColumn(action->CI);
		int containerB = state->topColumn(action->CF);

		//cout << action->CI << "," << action->CF << endl;

		double value = 1;

		double fp;
		if(!state->comprobarColumna(action->CI))
			fp=1.0;
		else
			fp=1.1-((state->heightColumn(action->CI)/(double)state->getHeight()) * containerA/(double)state->getMaxPriority()  );

		if(containerA >= containerB)
			value = 1 + (1- ((double)(containerA - containerB)/state->getMaxPriority()))*fp;
		if(state->comprobarColumna(action->CF) && containerA <= containerB)
			value = 2 -((double)(containerB-containerA)/state->getMaxPriority());

		//cout << value << endl;
		return value;
	}
};

}

#endif /* CPMPEVALUATOR_H_ */
