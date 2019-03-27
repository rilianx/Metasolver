/*
 * Greedy.h
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

#include "../ActionEvaluator.h"
#include "../SearchStrategy.h"


#ifndef GREEDY_H_
#define GREEDY_H_

using namespace std;

namespace metasolver {

class Greedy : public SearchStrategy {
public:

	//agregado alpha y beta para hacer funcionar el BeamACO
	Greedy(ActionEvaluator* evl,  double aco_alpha=0.0, double aco_beta=0.0, tau_matrix* tauM=NULL) :
		SearchStrategy(evl, aco_alpha, aco_beta, tauM) { }

	/**
	 * Performs an iteration of the strategy
	 */
	virtual list<State*> next(list<State*>& S) ;

};


} /* namespace clp */

#endif /* GREEDY_H_ */
