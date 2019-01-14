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

using namespace std;

namespace clp {

class VCS_Function : public VLossFunction{
public:
	VCS_Function(map<const BoxShape*, int>& nb_boxes, Vector3& dims, double alpha=4.0, double beta=1.0,
			double gamma=0.2, double p=0.04, double delta=1.0, double theta=0.0, double r=0.0, double max_theta=1.5, double alphafactor =1.0, double betafactor = 1.0, double gammafactor = 1.0, double deltafactor = 1.0, double pfactor = 1.0);

	virtual ~VCS_Function();

	virtual double eval_action(const State& , const Action& a );


	long cs[6];

	static int nn;

	double CS_p(const State& s, const Block& b, const Space& sp);


	virtual void set_alpha(double a){ alpha = a; }

	virtual void set_gamma(double g){ gamma = g; }

	virtual void set_p(double pp){ p = pp; }

	virtual void update_parameters(const State& s){

			int size = s.get_path().size(); //largo del camino
			const clpState* state=dynamic_cast<const clpState*>(&s);
			state->cont->nb_boxes; //numero cajas
			double fill = state->get_value();//porcentaje de llenado
			state->cont->getH();//Altura del contenedor
			const Space& sp= *dynamic_cast<SpaceSet*>(state->cont->spaces)->data.begin();


			if(size == 0){
				alpha = alpha0;
				set_beta(beta0);
				gamma = gamma0;
				p = p0;
				set_delta(delta0);
			}
			else{
				alpha = alpha0*pow(alphafactor,size);
				set_beta(beta0*pow(betafactor,size));
				gamma = gamma0*pow(gammafactor,size);
				p = p0*pow(pfactor,size);
				set_delta(delta0*pow(deltafactor,size));
			}
			//cout << "OLD: " << alpha << " " << get_beta() << " " << gamma << " " << p << " " << get_delta() << endl;
			//alpha *= 0.99;
			//beta *= 1.05;
			//set_beta(get_beta()*1.01);
			//gamma *= 0.99;
			//p *= 1.01;
			//set_delta(get_delta()*0.99);
			//delta *= 0.95;
			//cout << "NEW: " << alpha << " " << get_beta() << " " << gamma << " " << p << " " << get_delta() << endl;

		}

protected:

	long _surface_in_contact(const AABB& b, const AABB& bi);
	long _surface_in_contact(const AABB& bi, const Block& c);

	//parameters
  double alpha, gamma, p; //
  double alpha0;
  double gamma0;
  double p0;
  double beta0;
  double delta0;
  double alphafactor, betafactor, gammafactor, deltafactor, pfactor;


};

} /* namespace clp */

#endif /* HVCS_FUNCTION_H_ */
