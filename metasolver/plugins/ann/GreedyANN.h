/*
 * GreedyANN.h
 *
 *  Created on: Sep 25, 2019
 *      Author: iaraya
 */

#include "ActionEvaluator.h"
#include "SearchStrategy.h"
#include "Greedy.h"


#ifndef GREEDYANN_H_
#define GREEDYANN_H_

using namespace std;

namespace metasolver {

class GreedyANN : public Greedy {
public:

	GreedyANN(ActionEvaluator* evl, int n=100) : Greedy(evl), n(n), prev_pred(0.0) { }

	/**
	 * Performs an iteration of the strategy
	 */
	virtual list<State*> next(list<State*>& S) ;

	int n;

	double prev_pred;

};


} /* namespace clp */

#endif /* GREEDY_H_ */
