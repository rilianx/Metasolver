/*
 * GreedyANN.h
 *
 *  Created on: Sep 25, 2019
 *      Author: iaraya
 */

#include "ActionEvaluator.h"
#include "SearchStrategy.h"
#include "Greedy.h"


#ifndef GREEDYANNC_H_
#define GREEDYANNC_H_

using namespace std;

namespace metasolver {

/*
 * Greedy that uses the ANN classifier (vcs, vloss, cs) for selecting
 * the evaluation function in each iteration
 */

class GreedyANN_C : public Greedy {
public:

	GreedyANN_C(ActionEvaluator* evl, bool random=false) : Greedy(evl), random(random) { }

	/**
	 * Performs an iteration of the strategy
	 */
	virtual list<State*> next(list<State*>& S) ;

	bool random;

};


} /* namespace clp */

#endif /* GREEDY_H_ */
