/*
 * VCS.h
 *
 *  Created on: 2 jun. 2017
 *      Author: iaraya
 */

#ifndef VCS_FUNCTION_H_
#define VCS_FUNCTION_H_

#include "VLossFunction.h"
#include <vector>

using namespace std;

namespace clp {

class VCS_Function : public VLossFunction{
public:
	VCS_Function(map<const BoxShape*, int>& nb_boxes, Vector3& dims, double alpha=4.0, double beta=1.0,
			double gamma=0.2, double p=0.04, double delta=1.0, double theta=0.0, double r=0.0, double max_theta=1.5);

	void add_configuration(double alpha, double beta, double gamma, double p, double delta){
		configurations.push_back(vector<double>({alpha, beta, gamma, p, delta}));
	}

	virtual void use_configuration(int i){
		if(i>=configurations.size()){
			cout << "error: configuration " << i << "is out of range ("<<configurations.size()<< endl;
			exit(0);
		}
		alpha=configurations[i][0];
		beta=configurations[i][1];
		gamma=configurations[i][2];
		p=configurations[i][3];
		delta=configurations[i][4];
	}

	virtual ~VCS_Function();

	virtual double eval_action(const State& , const Action& a );

	double get_alpha() const{return alpha;}
	double get_beta() const{return beta;}
	double get_gamma() const{return gamma;}
	double get_delta() const{return delta;}
	double get_p() const{return p;}


	long cs[6];

	static int nn;

	double CS_p(const State& s, const Block& b, const Space& sp);


	virtual void set_alpha(double a){ alpha = a; }

	virtual void set_gamma(double g){ gamma = g; }

	virtual void set_p(double pp){ p = pp; }

protected:

	long _surface_in_contact(const AABB& b, const AABB& bi);
	long _surface_in_contact(const AABB& bi, const Block& c);

	//parameters
  double alpha, beta, gamma, delta, p; //
  vector < vector<double> > configurations;


};

} /* namespace clp */

#endif /* HVCS_FUNCTION_H_ */
