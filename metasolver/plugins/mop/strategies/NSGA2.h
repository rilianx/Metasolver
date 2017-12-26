/*
 * NSGA2.h
 *
 *  Created on: 18 oct. 2017
 *      Author: iaraya
 */

#ifndef PLUGINS_MOP_STRATEGIES_NSGA2_H_
#define PLUGINS_MOP_STRATEGIES_NSGA2_H_

#include <list>
#include "SearchStrategy.h"

using namespace metasolver; /* namespace clp */

class NSGA2 {
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

	public:
		NSGA2::NSGA2(double p_mut=0.3, double p_cross=0.9, double pop_size) : p_mut(p_mut), p_cross(p_cross), pop_size(pop_size) { };

		virtual NSGA2::~NSGA2() { };

		/**
		 * Performs an iteration of the strategy
		 * @returns true if the search strategy has not finished yet
		 */
		virtual std::list<Chromosome*> next(vector<Chromosome*>& G);

		/**
		 * compute the objective values of the chromosomes
		 */
		void NSGA2::calculate_objectives(list<Chromosome*>& children);

		//TODO: filter_nondominated_sort
		/**
		 * The filtered chromosomes should be deleted
		 */
		std::vector<Chromosome*> filter_nondominated_sort(list<Chromosome*>& G);


		void NSGA2::binary_tournament_selection(vector<Chromosome*>& pop, vector<Chromosome*>& selection);

		void NSGA2::setP_mut(double p_mut);
		void NSGA2::setP_cross(double p_cross);
		void NSGA2::setPop_size(int pop_size);

		double NSGA2::getP_mut();
		double NSGA2::getP_cross();
		int NSGA2::getPop_size();

};

#endif /* PLUGINS_MOP_STRATEGIES_NSGA2_H_ */
