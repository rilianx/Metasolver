/*
 * MOP_VCS_Function.h
 *
 *  Created on: 21 sept. 2017
 *      Author: iaraya
 */

#ifndef CLP_PLUGINS_MOP_MO_VCS_FUNCTION_H_
#define CLP_PLUGINS_MOP_MO_VCS_FUNCTION_H_

#include "../../evaluators/VCS_Function.h"

using namespace std;

namespace clp {

class MOP_VCS_Function : public VCS_Function, MO_ActionEvaluator{

	MOP_VCS_Function(map<const BoxShape*, int>& nb_boxes, Vector3& dims, double alpha=4.0, double beta=1.0,
				double gamma=0.2, double p=0.04, double delta=1.0, double r=0.0) :
		VCS_Function(nb_boxes, dims, alpha, beta, gamma, p, delta, r) { }

};



#endif /* CLP_PLUGINS_MOP_MO_VCS_FUNCTION_H_ */
