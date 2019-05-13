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

	enum aco_strategy {ONE_ARC=1, ONE_PATH, ALL_PATHS};

	/**
	 * Constructor
	 * @param greedy The underlying greedy algorithm
	 * @param expander
	 * @param beams the number of beams
	 * @p_elite the proportion of beams in the elite set (0.0, means 1 beam)
	 * @max_level_size the maximum number of expanded nodes by level of the tree
	 */
	BeamACO(ActionEvaluator* evl, SearchStrategy& greedy, int beams, double p_elite=0.0,
			int max_level_size=0, bool plot=false, double aco_alpha=0.0, double aco_beta=0.0,
			tau_matrix* tauM=NULL, aco_strategy metodo=ONE_ARC, double mod_factor=0.0, double incremento= 0.0) :
				BSG(evl, greedy, beams, p_elite, max_level_size, plot, aco_alpha, aco_beta, tauM),
				metodo(metodo), mod_factor(mod_factor), incremento(incremento), update_ph(true) {

	}


	virtual ~BeamACO();

	/**
	 * Performs an iteration of the strategy
	 */
	virtual list<State*> next(list<State*>& S) ;

	double incremento;
	aco_strategy metodo;
	double mod_factor;

	bool update_ph;

};

} /* namespace clp */

#endif /* STRATEGIES_BSG_H_ */
