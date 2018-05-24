/*
 * cpmpState.h
 *
 *  Created on: 24 may. 2018
 *      Author: iaraya
 */

#include "State.h"

#ifndef CPMPSTATE_H_
#define CPMPSTATE_H_

using namespace metasolver;

namespace cpmp {

class cpmpState : public State {
public:

	cpmpState();
	virtual ~cpmpState();

	/**
	 * The value of the objective function
	 */
	virtual double get_value() const = 0;

	virtual void get_actions(list< Action* >& actions) const = 0;

	virtual void _transition(const Action& action) = 0;



};

} /* namespace cpmp */

#endif /* CPMPSTATE_H_ */
