/*
 * BSGpath.h
 *
 *  Created on: 12 jul. 2017
 *      Author: iaraya
 */

#ifndef SIMULATION_H_
#define SIMULATION_H_

#include "Greedy.h"


namespace metasolver {

/**
 * Perform a simulation.
 * After following a list of actions, performs a greedy from the reached state
 */

class Simulation : public Greedy {
public:
	/**
	 * Constructor
	 * @param greedy The underlying greedy algorithm
	 * @param expander
	 * @param beams the number of beams
	 * @p_elite the proportion of beams in the elite set (0.0, means 1 beam)
	 * @max_level_size the maximum number of expanded nodes by level of the tree
	 */
	Simulation(ActionEvaluator* evl) :
		Greedy(evl) {}

	void set_actions(list<int>& actions){
		this->actions = actions;
	}

	virtual ~Simulation();

	/**
	 * Performs an iteration of the strategy
	 */
	virtual list<State*> next(list<State*>& S) ;

    list<int> actions;

};

} /* namespace clp */

#endif /* STRATEGIES_BSG_H_ */
