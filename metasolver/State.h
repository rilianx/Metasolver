/*
 * State.h
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

#include <map>
#include <string>
#include <list>
#include <iostream>
#include <functional>
#include <math.h>

#include "boost/math/distributions/normal.hpp"
#include <boost/math/distributions/students_t.hpp>



#ifndef STATE_H_
#define STATE_H_


#include "ActionEvaluator.h"
#include <algorithm>


using namespace std;
using namespace boost::math;

namespace metasolver{

class Action{
public:
	virtual Action* clone() const=0;

	virtual bool operator!=(const Action& B) const{
		return !(*this==B);
	}

  virtual bool operator==(const Action& B) const{
		cout << "Error: the operator== is not implemented in the corresponding Action subclass" << endl;
		exit(0);
		return false;
	};

	virtual void serialize(ostream& os) const{  }

	virtual ~Action(){}

};

ostream & operator << (ostream &out, const Action &a);

/**
 * Represent a partial solution of the problem. It consists in
 * the container (block) with the located blocks, a map of the left boxes and
 * a list of valid blocks which can be put in the container.
 */
class State {
public:

	State() : parent (NULL), id(count_states++), var(1e-6), mean(0.0){}

	virtual State* clone() const = 0;


	State(const State& S) : parent(&S), id(count_states++), var(S.var), mean(0.0), promise(0.0), children_size(0), nb_simulations(0){

		list<const Action*>::iterator it=S.get_path().begin();
		for(;it!=S.path.end();it++)
			path.push_back((*it)->clone());
	}

	virtual State* create_neighbor(State* s0){
		return NULL;
	}

	virtual ~State() {
		while(!path.empty()){
			delete path.front();
			path.pop_front();
		}
	}

	/**
	 * The value of the objective function
	 */
	virtual double get_value() const = 0;

	/**
	 * The value of the second objective function
	 */
	virtual double get_value2() const { return 0.0; }

	void transition(const Action& action) {
		path.push_back(action.clone());
		_transition(action);
	}

	/*
	* Rearranges the elements in the path randomly
	*/
	virtual int shuffle_path(){

	    return 0;
	   // std::random_shuffle ( path.begin(), path.end() );
	}

	// Probabilidad de generar simulaciones mejores a la mejor de acuerdo a mean y sd
	//FIXME: THIS CLASS?

	void calculate_promise(double best_value) const{
		//cout << children_size << endl;
		if(var==0.0) {promise=0; return;}

		//promise = mean + eps* var;

  //mcts bacan
		double z_value=stadistic_test(best_value);
    students_t dist  (children_size);
    promise= 1.0-cdf(dist, z_value);
		
   //mcts profe
    //normal dist( mean, sqrt(var) );
		//promise= 1.0-cdf(dist, best_value);


		//cout << mean << "," << var <<"," << best_value << ":" << promise << endl;

	}


	double stadistic_test(double best_value) const {

		double t = ( ( mean - best_value) / sqrt(var) );


	    t = t* sqrt(children_size); //FIXME: SQRT ? POW


		return t;
	}



	virtual void get_actions(list< Action* >& actions) const = 0;

	virtual Action* next_action(State& final);

	bool is_root(){ return (path.size()==0); }

	list<const Action*>& get_path() const{ return path;}

	virtual void print() {  }

	// Actualiza los valores mean y sd de acuerdo al nuevo valor

	virtual void update_values(double new_value, bool bp, bool first=true) const{
		if(first)
		   children_size++;

		nb_simulations++;

		if(nb_simulations >= 1){
			//mean = max(mean,new_value);
			mean = (mean*(nb_simulations-1)+new_value)/nb_simulations;
		}

		if(nb_simulations>=2){
			double varr= ((var*(nb_simulations-2)) + pow( (new_value-mean),2) ) / ( nb_simulations-1 ); //actualiza la varianza

			//si la varianza es menor a la del padre se resetea a la del padre
			var=varr;
		}

    //TODO: arreglar back-propagation. 
		if(bp && parent)
		   parent->update_values(new_value, true, false);


	}

	inline double get_promise() const{return promise;}

	inline void set_promise(double p) const{promise=p;}

	inline void set_mean(double m) const{mean=m;}

	double get_mean() const {return mean;}
	double get_var() const {return var;}

	const list<const State*>& get_children() const { return children;}

	const int get_children_size() const { return children_size;}

	void add_children(State* s) const{ children.push_back(s);}


	int get_id() const { return id; }

	const State* get_parent() const{ return parent; }

    void remove_children  (const State* ch) const{
		children.remove(ch);
	}

    int remove_children  () const{
    	int s=children.size();
    	for(auto ch:children)
    		delete ch;

		children.clear();
		return s;
	}

    void remove_parent () const{
    	parent=NULL;
	}

	static int count_states;



protected:

	virtual void _transition(const Action& action) = 0;

	mutable const State* parent;
	//bool root;

	//list of actions for reconstructing the state from scratch
	mutable list<const Action*> path;
	mutable list<const State*> children;
	mutable double var; // variance
	mutable double mean; // mean arithmetics
	//double sd; // standard deviation
	mutable double promise; // value can be the best of the best

	mutable int children_size;

	mutable int nb_simulations;

	int id;
};


}

#endif /* STATE_H_ */
