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
			double gamma=0.2, double p=0.04, double delta=1.0, double delta2=0.0, double delta3=0.0, double r=0.0);




	virtual ~VCS_Function();

	virtual double eval_action(const State& , const Action& a );

	//sets the parameter values for maximizing the second objective
	virtual void set_parameters (double alpha, double beta,
			double gamma, double p, double delta, double delta2, double delta3){

		this->alpha=alpha;
		this->beta=beta;
		this->gamma=gamma;
		this->p=p;
		this->delta=delta;
		this->delta2=delta2;
		this->delta3=delta3;
	}

	//sets the parameter values for maximizing the second objective
	virtual void set_parameters2 (double alpha, double beta,
			double gamma, double p, double delta, double delta2, double delta3){

		alpha_2=alpha;
		beta_2=beta;
		gamma_2=gamma;
		p_2=p;
		delta_2=delta;
		delta2_2=delta2;
		delta3_2=delta3;
	}


	long cs[6];

	static int nn;

	double CS_p(const State& s, const Block& b, const Space& sp, double p);

protected:

	long _surface_in_contact(const AABB& b, const AABB& bi);
	long _surface_in_contact(const AABB& bi, const Block& c);

	//parameters
  double alpha, beta, gamma, p, delta, delta2, delta3; // maximizing obj1
  double alpha_2, beta_2, gamma_2, p_2, delta_2, delta2_2, delta3_2; // maximizing obj2


};

} /* namespace clp */

#endif /* HVCS_FUNCTION_H_ */
