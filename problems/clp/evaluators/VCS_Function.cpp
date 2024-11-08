/*
 * VCS.cpp
 *
 *  Created on: 2 jun. 2017
 *      Author: iaraya
 */

#include <iostream>
#include <math.h>
#include "VLossFunction.h"
#include "VCS_Function.h"
#include "../clpState.h"
#include "../objects2/AABB.h"
#include "../objects2/Block.h"
#include "../../metasolver/GlobalVariables.h"

using namespace std;

namespace clp {

VCS_Function::VCS_Function(map<const BoxShape*, int>& nb_boxes, Vector3& dims, double alpha, double beta,
		double gamma, double p, double delta, double delta2, double delta3, double r):
		VLossFunction (nb_boxes, dims, r), alpha(alpha),
		beta(beta), gamma(gamma), p(p), delta(delta), delta2(delta2), delta3(delta3),
		alpha_2(alpha),beta_2(beta), gamma_2(gamma), p_2(p), delta_2(delta),
		delta2_2(delta2), delta3_2(delta3){ }

VCS_Function::~VCS_Function(){

}




double VCS_Function::eval_action(const State& s, const Action &a){
	const Block& b = dynamic_cast<const clpAction*>(&a)->block;
	const Space& sp =dynamic_cast<const clpAction*>(&a)->space;
	const clpState* ss =dynamic_cast<const clpState*>(&s);

    long resL=sp.getL() - b.getL();
    long resW=sp.getW() - b.getW();
    long resH=sp.getH() - b.getH();


    if(resL<0 || resW<0 || resH<0) return -1.0;
    if(clpState::Wmax > 0.0 && ss->cont->getTotalWeight() + b.getTotalWeight() > clpState::Wmax) return -1.0;


    double alpha=this->alpha + lambda2*(this->alpha_2 - this->alpha);
    double beta=this->beta + lambda2*(this->beta_2 - this->beta);
    double gamma=this->gamma + lambda2*(this->gamma_2 - this->gamma);
    double p=this->p + lambda2*(this->p_2 - this->p);
    double delta=this->delta + lambda2*(this->delta_2 - this->delta);
    double delta2=this->delta2 + lambda2*(this->delta2_2 - this->delta2);
    double delta3=this->delta3 + lambda2*(this->delta3_2 - this->delta3);

	double loss=(beta>0.0)?
			Loss(dynamic_cast<const clpState*>(&s)->nb_left_boxes, b, sp) : 0.0;

	double vol=(delta>0.0)? (double) b.getOccupiedVolume(): 1.0;

	double cs=(alpha>0.0)? CS_p(s, b, sp, p) : 1.0;

	double n=(gamma>0.0)? (1.0/(double) b.n_boxes) : 1.0;

	dynamic_cast<const clpAction*>(&a)->metrics.push_back(vol);
	dynamic_cast<const clpAction*>(&a)->metrics.push_back(loss);
	dynamic_cast<const clpAction*>(&a)->metrics.push_back(cs);
	dynamic_cast<const clpAction*>(&a)->metrics.push_back(n);

	double eval;
	if(clpState::Wmax > 0.0){
		double density = b.getTotalWeight() / (double) b.getVolume();
		double profit = b.getTotalProfit();

		eval =( pow(vol, delta)  * pow((1.0-loss), beta) * pow(cs, alpha) *
				     pow(n,gamma) * pow(density, delta2) * pow(profit, delta3));
	}


	eval= (pow(vol, delta)  * pow((1.0-loss),beta) * pow(cs,alpha) * pow(n,gamma) );
	dynamic_cast<const clpAction*>(&a)->metrics.push_back(eval);

	return eval;
}

double VCS_Function::CS_p(const State& s, const Block& b, const Space& sp, double p){
	   long surface=0;

	   Vector3 oo=sp.get_location(b);

	   Vector3 diff=Vector3( p*(double) b.getL(),p*(double) b.getW(),p*(double) b.getH() );


	   AABB bb(oo, oo + b);
	   AABB bexp(oo - diff, oo + b + diff );


	   for(int i=0; i<6; i++) cs[i]=0.0;

	   std::list<const AABB*> blocks =
			   dynamic_cast<const clpState*>(&s)->cont->blocks->get_intersected_objects(bexp);

	   std::list<const AABB*>::iterator it;

	   for(it=blocks.begin(); it!=blocks.end(); it++){
		   surface=surface+_surface_in_contact(bb,**it);
	   }

	   surface=surface+_surface_in_contact(bb, *dynamic_cast<const clpState*>(&s)->cont);

	   return (double)surface / (double)(bb.getSurface());
}


long VCS_Function::_surface_in_contact(const AABB& b, const AABB& bi){
   long x_min = std::max(b.getXmin(), bi.getXmin());
   long x_max = std::min(b.getXmax(), bi.getXmax());
   long y_min = std::max(b.getYmin(), bi.getYmin());
   long y_max = std::min(b.getYmax(), bi.getYmax());
   long z_min = std::max(b.getZmin(), bi.getZmin());
   long z_max = std::min(b.getZmax(), bi.getZmax());

   long s=0;
   if(y_max>y_min && z_max>z_min){
		  if(b.getXmax() >= bi.getXmin() - p*(double)b.getL()){
			  s=(y_max-y_min)*(z_max-z_min);
			  cs[0]+=s;
		  }else if(bi.getXmax() >= b.getXmin() - p*(double)b.getL()){
			  s=(y_max-y_min)*(z_max-z_min);
			  cs[0]+=s;
		  }
    }else if(x_max>x_min && z_max>z_min){
	      if(b.getYmax() >= bi.getYmin() - p*(double)b.getW()){
	    	  s=(x_max-x_min)*(z_max-z_min);
			  cs[1]+=s;
	      }else if(bi.getYmax() >= b.getYmin() - p*(double)b.getW()){
	    	  s=(x_max-x_min)*(z_max-z_min);
			  cs[1]+=s;
	      }
    }else if(x_max>x_min && y_max>y_min){
	      if(b.getZmax() >= bi.getZmin() - p*(double)b.getH()){
	    	  s=(x_max-x_min)*(y_max-y_min);
			  cs[2]+=s;
	      }else if(bi.getZmax() >= b.getZmin() - p*(double)b.getH()){
	    	  s=(x_max-x_min)*(y_max-y_min);
			  cs[2]+=s;
		 }
	}

	return s;
}

long VCS_Function::_surface_in_contact(const AABB& b, const Block& cont){
	   long surface=0, s;

	   if(b.getXmin() <= p*(double)b.getL() ){
		   s=(b.getW() * b.getH());
		   surface += s;
		   cs[0]+=s;
	   }

	   if(b.getXmax() >= cont.getL() - p*(double)b.getL() ){
		   s=(b.getW() * b.getH());
		   surface += s;
		   cs[0]=max(s,cs[0]);
	   }

	   if(b.getYmin() <= p*(double)b.getW() ){
		   s = (b.getL() * b.getH());
		   surface += s;
		   cs[1]+=s;
	   }
	   if(b.getYmax() >= cont.getW() - p*(double)b.getW() ){
		   s = (b.getL() * b.getH());
		   surface += s;
		   cs[1]+=max(s,cs[1]);
	   }

	   if(b.getZmin() <= p*(double)b.getH() ){
		   s = (b.getL() * b.getW());
		   surface += s;
		   cs[2]+=s;
	   }

	   if(b.getZmax() >= cont.getH() - p*(double)b.getH() ){
		   s = (b.getL() * b.getW());
		   surface += s;
		   cs[2]+=max(s,cs[2]);
	   }

	   return surface;
}



} /* namespace clp */
