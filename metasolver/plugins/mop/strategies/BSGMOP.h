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

class BSG_MOP : public SearchStrategy{
public:
	BSG_MOP();
	virtual ~BSG_MOP();

	/**
	 * Performs an iteration of the strategy
	 * @returns true if the search strategy has not finished yet
	 */
	virtual list<State*> next(list<State*>& S)=0 ;

private:

	list<State*> NDS;
};

} /* namespace clp */

#endif /* PLUGINS_MOP_BSGMOP_H_ */
