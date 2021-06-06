/*
 * VCS.h
 *
 *  Created on: 2 jun. 2017
 *      Author: iaraya
 */

#ifndef VCS_FUNCTION_H_
#define VCS_FUNCTION_H_

#include "VLossFunction.h"
#include "clpState.h"
#include <vector>

using namespace std;

namespace clp {

class VCS_Function : public VLossFunction{
public:
	VCS_Function(map<const BoxShape*, int>& nb_boxes, Vector3& dims, double alpha=4.0, double beta=1.0,
			double gamma=0.2, double p=0.04, double delta=1.0, double delta2=0.0, double delta3=0.0, double r=0.0);


			VCS_Function(map<const BoxShape*, int>& nb_boxes, Vector3& dims, vector<double>& theta, double r=0.0) :
			VCS_Function(nb_boxes, dims, theta[4], theta[5], theta[6], theta[3], theta[0],  theta[2], theta[1], r) {  }


	virtual ~VCS_Function();

	virtual double eval_action(const State& , const Action& a );

	//sets the parameter values for maximizing the second objective
	virtual void set_parameters (vector<double>& theta){
			alpha=theta[4];
			beta=theta[5];
			gamma=theta[6];
			p=theta[3];
			delta=theta[0];
			delta2=theta[2];
			delta3=theta[1];
	}

	//sets the parameter values for maximizing the second objective
	virtual void set_parameters2 (vector<double>& theta){
		alpha_2=theta[4];
		beta_2=theta[5];
		gamma_2=theta[6];
		p_2=theta[3];
		delta_2=theta[0];
		delta2_2=theta[2];
		delta3_2=theta[1];
	}


	long cs[6];

	static int nn;

	double CS_p(const State& s, const Block& b, const Space& sp, double p);
	double min_contact_surface(const clpState& s, const AABB& aabb);

protected:

	long _surface_in_contact(const AABB& b, const AABB& bi);
	long _surface_in_contact(const AABB& bi, const Block& c);

	//parameters
  double alpha, beta, gamma, p, delta, delta2, delta3; // maximizing obj1
  double alpha_2, beta_2, gamma_2, p_2, delta_2, delta2_2, delta3_2; // maximizing obj2


};

} /* namespace clp */

#endif /* HVCS_FUNCTION_H_ */
