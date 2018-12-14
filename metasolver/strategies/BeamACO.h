/*
 * BSGpath.h
 *
 *  Created on: 12 jul. 2017
 *      Author: iaraya
 */

#ifndef STRATEGIES_BEAMACO_H_
#define STRATEGIES_BEAMACO_H_

#include "SearchStrategy.h"
#include "BSG.h"

namespace metasolver {


/**
 * Variante del beam search que utiliza una lista the best_path
 * para guiarse en la busqueda
 */
class BeamACO : public BSG {
public:
	/**
	 * Constructor
	 * @param greedy The underlying greedy algorithm
	 * @param expander
	 * @param beams the number of beams
	 * @p_elite the proportion of beams in the elite set (0.0, means 1 beam)
	 * @max_level_size the maximum number of expanded nodes by level of the tree
	 */
	BeamACO(ActionEvaluator* evl, SearchStrategy& greedy, int beams, double p_elite=0.0,
			int max_level_size=0, bool plot=false, double aco_alpha=0.0, double aco_beta=0.0) :
				BSG(evl, greedy, beams, p_elite, max_level_size, plot, aco_alpha, aco_beta) {

	}


	virtual ~BeamACO();

	/**
	 * Performs an iteration of the strategy
	 */
	virtual list<State*> next(list<State*>& S) ;




};

} /* namespace clp */

#endif /* STRATEGIES_BSG_H_ */