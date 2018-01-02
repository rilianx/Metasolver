/*
 * clp_chromosome.h
 *
 *  Created on: 27 dic. 2017
 *      Author: iaraya
 */

#include "Chromosome.h"

#ifndef PROBLEMS_CLP_PLUGINS_GA_CLP_CHROMOSOME_H_
#define PROBLEMS_CLP_PLUGINS_GA_CLP_CHROMOSOME_H_

using namespace std;

namespace clp {

class GenCLP : public Gen{

public:

	double random(double min, double max){
		return min + ((double)rand()/(double)RAND_MAX)*(max-min);
	}

	GenCLP(){
		rank=1;//rand() % max_rank + 1;
		//alpha= random(min_alpha,max_alpha);
		//beta= random(min_beta,max_beta);
		//gamma= random(min_gamma,max_gamma);
		//p= random(min_p,max_p);
		//theta= random(min_theta,max_theta);
	}

	virtual ~GenCLP(){ }

	virtual Gen* clone();

	virtual void mutate();

	virtual void mutate2();

	virtual void write(std::ostream&) const;

	int rank;

	static int max_rank;

};


class ChromosomeCLP : public Chromosome{
public:

	/** Create a random chromosome of n genes */
	ChromosomeCLP(ActionEvaluator* evl) : Chromosome(evl){

	}

	ChromosomeCLP(ChromosomeCLP& c) : Chromosome(c.evl){
		for(auto g:c.genes){
			genes.push_back(g->clone());
		}
	}

	/*
	 * Initialize the variables of the specific strategy
	 */
	virtual void initialize (State* s=NULL) {
		if(best_state) delete best_state;
		best_state=s;
	}

	virtual Chromosome* clone();

	virtual ~ChromosomeCLP();

	virtual void write(std::ostream&) const;

	/**
	 * Run the strategy
	 */
	virtual double run(State& s, double tl=99999.9, clock_t bt=clock());



};




class ChromosomeFactoryCLP: public ChromosomeFactory{

public:

	virtual Chromosome* generate_random_chromosome(ActionEvaluator* evl, State& s);
};


}

#endif /* PROBLEMS_CLP_PLUGINS_GA_CLP_CHROMOSOME_H_ */
