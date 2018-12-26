/*
 * GreedyACO.h
 *
 *  Created on: 20 dic. 2018
 *      Author: emiliano
 */

#include "../ActionEvaluator.h"
#include "../SearchStrategy.h"

#ifndef STRATEGIES_GREEDYACO_H_
#define STRATEGIES_GREEDYACO_H_

using namespace std;

namespace metasolver {

class GreedyACO : public SearchStrategy {
public:

	GreedyACO(ActionEvaluator* evl, double alpha, double beta, tau_matrix* tauM) : SearchStrategy(evl,alpha,beta,tauM) { }

	/**
	 * Performs an iteration of the strategy
	 */
	virtual list<State*> next(list<State*>& S) ;

};


} /* namespace clp */

#endif /* STRATEGIES_GREEDYACO_H_ */
