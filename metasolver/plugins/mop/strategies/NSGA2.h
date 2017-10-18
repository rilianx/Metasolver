/*
 * NSGA2.h
 *
 *  Created on: 18 oct. 2017
 *      Author: iaraya
 */

#ifndef PLUGINS_MOP_STRATEGIES_NSGA2_H_
#define PLUGINS_MOP_STRATEGIES_NSGA2_H_

#include <list>
#include "../../../SearchStrategy.h"

namespace clp {


class Gen {

public:
	int type; //type of box
	int n; //number of boxes
	int o; //orientation
};

class Chromosome {

public:
	Chromosome(list<Gen>& genes) : genes(genes){ }

	Chromosome* copy() {
		return new Chromosome(genes);
	}

	/*
	 * Mutates the chromosome
	 */
	Chromosome* mutate(){
		Chromosome* c_new = copy();

		//modifica sus genes de acuerdo a alguna probabilidad

		return c_new;
	}


	/**
	* The random movements used by the mutation operator
	*/
	void add_rnd_gene();
	void remove_rnd_gene();
	void change_rnd_gene();

	/**
	 * TODO: (Ignacio) Evaluate the objectives
	 */
	double get_value();
	double get_value2();


	list<Gen> genes;

};

class NSGA2 {
public:


	NSGA2(double p_mut=0.3, double p_cross=0.9) : p_mut(p_mut), p_cross(p_cross) { };

	virtual ~NSGA2() { };

	/**
	 * Performs an iteration of the strategy
	 * @returns true if the search strategy has not finished yet
	 */
	virtual list<Chromosome*> next(list<Chromosome*>& G);

	/**
	 * The mutation operator
	 */
	virtual list<Chromosome*> mutation(list<Chromosome*>& G);

	/**
	 * The crossover operator
	 */
	virtual list<Chromosome*> crossover(list<Chromosome*>& G);




private:

	/**
	 * Mutation probability
	 */
	double p_mut;

	/**
	 * Crossover probability
	 */
	double p_cross;

};

} /* namespace clp */

#endif /* PLUGINS_MOP_STRATEGIES_NSGA2_H_ */
