/*
 * BSGMOP.h
 *
 *  Created on: 13 sept. 2017
 *      Author: iaraya
 */

#ifndef PLUGINS_MOP_BSGMOP_H_
#define PLUGINS_MOP_BSGMOP_H_

#include <list>
#include "../../../SearchStrategy.h"


namespace clp {

class BSG_MOP : public BSG{
public:
	/**
	 * Constructor
	 * @param greedy The underlying greedy algorithm
	 * @param expander
	 * @param beams the number of beams
	 * @p_elite the proportion of beams in the elite set (0.0, means 1 beam)
	 * @max_level_size the maximum number of expanded nodes by level of the tree
	 */
	BSG_MOP(SearchStrategy& greedy, int beams, MO_ActionEvaluator& evl, double p_elite=0.0, int max_level_size=0) :
		BSG(greedy, beams, p_elite, max_level_size), evl(evl){

	}

	virtual ~BSG_MOP();

	/**
	 * Performs an iteration of the strategy
	 * @returns true if the search strategy has not finished yet
	 */
	virtual list<State*> next(list<State*>& S)=0 ;

private:

	//conjunto de soluciones no dominadas
	list<State*> NDS;

	//evaluador de acciones con parametro alpha
	MO_ActionEvaluator& evl;

	void select_coeff(list<double>& coeff, int n);
};

} /* namespace clp */

#endif /* PLUGINS_MOP_BSGMOP_H_ */
