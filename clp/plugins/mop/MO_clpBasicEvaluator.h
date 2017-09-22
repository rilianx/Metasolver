/*
 * MO_clpBasicEvaluator.h
 *
 *  Created on: 21 sept. 2017
 *      Author: iaraya
 */

#ifndef CLP_PLUGINS_MOP_MO_CLPBASICEVALUATOR_H_
#define CLP_PLUGINS_MOP_MO_CLPBASICEVALUATOR_H_

#include "../../../metasolver/ActionEvaluator.h"

namespace clp {
/**
 * /brief an Action evaluator for MOPs
 *
 * The first objective is evaluated by ev1 and the second one is: total_weight (box)/occupied_volume (box)
 */
class MO_clpBasicEvaluator : public MO_ActionEvaluator{
public:
	MO_clpBasicEvaluator(ActionEvaluator& ev1, double r) : MO_ActionEvaluator(&ev1, NULL, r) { };

protected:

	virtual double eval_action2(const State& s, const Action& a){
		const Block& b = dynamic_cast<const clpAction*>(&a)->block;

        return b.getTotalWeight()/(double) b.getOccupiedVolume();

	}


};

}



#endif /* CLP_PLUGINS_MOP_MO_CLPBASICEVALUATOR_H_ */
