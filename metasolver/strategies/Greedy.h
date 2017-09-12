/*
 * Greedy.h
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

#include "../ActionEvaluator.h"
#include "../SearchStrategy.h"
#include "../Expander.h"

#ifndef GREEDY_H_
#define GREEDY_H_

using namespace std;

namespace clp {

class Greedy : public SearchStrategy {
public:

	Greedy(Expander &exp): expander(exp) { }



	/**
	 * Performs an iteration of the strategy
	 */
	virtual list<State*> next(list<State*>& S) ;


private:
	Expander &expander;
};

} /* namespace clp */

#endif /* GREEDY_H_ */
