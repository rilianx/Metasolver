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

namespace clp {

class Greedy : public SearchStrategy {
public:

	/**
	 * Performs an iteration of the strategy
	 */
	virtual list<State*> next(list<State*>& S) ;

};

class MO_Greedy : public Greedy {
public:

	virtual pair<double, double> run(MO_State& s, double tl=99999.9, clock_t bt=clock()){
		Greedy::run(s, tl, bt);

		return make_pair(s.get_value(), s.get_value2());
	}

};


} /* namespace clp */

#endif /* GREEDY_H_ */
