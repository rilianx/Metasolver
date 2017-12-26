/*
 * VCS.h
 *
 *  Created on: 2 jun. 2017
 *      Author: iaraya
 */

#ifndef VCS_FUNCTION_H_
#define VCS_FUNCTION_H_

#include "VLossFunction.h"

using namespace std;

namespace clp {

class VCS_Function : public VLossFunction{
public:
	VCS_Function(map<const BoxShape*, int>& nb_boxes, Vector3& dims, double alpha=4.0, double beta=1.0,
			double gamma=0.2, double p=0.04, double delta=1.0, double f=0.0, double r=0.0);

	virtual ~VCS_Function();

	virtual double eval_action(const State& , const Action& a );


	long cs[6];

	static int nn;

	double CS_p(const State& s, const Block& b, const Space& sp);

protected:

	long _surface_in_contact(const AABB& b, const AABB& bi);
	long _surface_in_contact(const AABB& bi, const Block& c);

	//parameters
  double alpha, gamma, p; //


};

} /* namespace clp */

#endif /* HVCS_FUNCTION_H_ */
