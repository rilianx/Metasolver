/*
 * Greedy.h
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

#include <cstdlib>
#include <math.h>
#include <vector>
#include "State.h"

#ifndef HEURISTIC_FUNCTION_H_
#define HEURISTIC_FUNCTION_H_

using namespace std;

namespace metasolver{

class State;
class Action;
/**
 * This class allows to evaluate blocks and spaces in order to
 * select the next action
 */
class ActionEvaluator {
public:
	ActionEvaluator(double r) : r(r), lambda2(0.0)  { };

	virtual ~ActionEvaluator() { }

	virtual double eval_action_rand(const State& s, const Action& a){

		double eval=eval_action(s, a);
		double ran=(r>0.0)? ((double) rand() / (double) RAND_MAX) : 1.0;
		if(r==0.0) return eval;
		return ( eval * pow(ran,r) );
	}

	virtual double eval_action(const State& s, const Action& a) =0;

	//set the weitght of the second objective
	virtual void set_lambda2(double l){
		lambda2 = l;
	}

	virtual double get_lambda2(){
		return lambda2;
	}

	double get_r() const { return r; }


	virtual void set_parameter_ranges(const vector< pair<double,double> >& v){
		parameter_ranges=v;
	}

	/*
	 * Retorna los rangos recomendados para los parámetros
	 * Necesario para BeamACO
	 */
	virtual const vector< pair<double,double> >& get_parameter_ranges() const{
		return parameter_ranges;
	}

	/*
	 * Retorna los valores asignados para los parámetros
	 */
	virtual vector< double > get_parameter_values() const = 0;

	/*
	 * Asigna los valores a los parámetros
	 */
	virtual void set_parameter_values(const vector< double >&) = 0;


protected:


	//parameter ranges
	vector< pair<double,double> > parameter_ranges;

   //weitght of the second objective (lambda1=1-lambda2)
   double lambda2;

	private:
	double r;


};



}

#endif /* HEURISTIC_FUNCTION_H_ */
