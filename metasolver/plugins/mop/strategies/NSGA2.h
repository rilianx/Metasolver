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

	virtual ~Chromosome() { };

	Chromosome* copy() {
		return new Chromosome(genes);
	}

	/*
	 * TODO: Operador de mutacion
	 * Mutates the chromosome
	 */
	Chromosome* mutate(){
		Chromosome* c_new = copy();

		//modifica sus genes de acuerdo a alguna probabilidad

		return c_new;
	}

	/*
	 * TODO: Operador de cruzamiento (Cristobal)
	 */
	Chromosome* crossover(Chromosome* c2){
		Chromosome* c_new = copy();

		//cruzar this con c2

		return c_new;
	}

	/**
	 * TODO: movimientos del operador de mutacion (Cristobal)
	 * The random movements used by the mutation operator
	 */
	void add_rnd_gene();
	void remove_rnd_gene();
	void change_rnd_gene();

	/**
	 * TODO: (Ignacio) Calculate the objectives
	 */
	virtual double get_value();
	virtual double get_value2();


	list<Gen> genes;

};

class NSGA2 {
public:


	NSGA2(double p_mut=0.3, double p_cross=0.9, double pop_size) : p_mut(p_mut), p_cross(p_cross), pop_size(pop_size) { };

	virtual ~NSGA2() { };

	/**
	 * Performs an iteration of the strategy
	 * @returns true if the search strategy has not finished yet
	 */
	virtual list<Chromosome*> next(vector<Chromosome*>& G);

	/**
	 * compute the objective values of the chromosomes
	 */
	void calculate_objectives(list<Chromosome*>& children){
		for(auto chrom : children){
			chrom->get_value();
			chrom->get_value2();
		}
	}

	//TODO: filter_nondominated_sort
	vector<Chromosome*> filter_nondominated_sort(list<Chromosome*>& G);


	void binary_tournament_selection(vector<Chromosome*>& pop, vector<Chromosome*>& selection);


private:

	/**
	 * Mutation probability
	 */
	double p_mut;

	/**
	 * Crossover probability
	 */
	double p_cross;

	/**
	 * Size of the population
	 */
	int pop_size;

};

} /* namespace clp */

#endif /* PLUGINS_MOP_STRATEGIES_NSGA2_H_ */
