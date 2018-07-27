/*
 * DoubleEffort.h
 *
 *  Created on: 6 jul. 2017
 *      Author: iaraya
 */

#ifndef STRATEGIES_DOUBLEEFFORT_H_
#define STRATEGIES_DOUBLEEFFORT_H_
#include "SearchStrategy.h"
#include <iostream>
#include <list>

using namespace std;

namespace metasolver {

class DoubleEffort : public SearchStrategy {
public:
	DoubleEffort(SearchStrategy& bsg) : bsg(bsg) {};

	virtual list<State*> next(list<State*>& S){
		State& s= **S.begin();

		bsg.run(*s.clone(), timelimit, begin_time);

		if(get_best_value() < bsg.get_best_value()){
			best_state=bsg.get_best_state()->clone();
			cout << "[DoubleEffort] new best_solution_found ("<< get_time() <<"): " << get_best_value() << endl;
		}


		if(!bsg.double_effort()){
			clean(S);
			return S;
		}

		return S;
	}

private:
	SearchStrategy& bsg;
};

} /* namespace clp */

#endif /* STRATEGIES_DOUBLEEFFORT_H_ */
