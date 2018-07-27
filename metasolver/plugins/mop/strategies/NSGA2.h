/*
 * NSGA2.h
 *
 *  Created on: 18 oct. 2017
 *      Author: iaraya
 */

#ifndef PLUGINS_MOP_STRATEGIES_NSGA2_H_
#define PLUGINS_MOP_STRATEGIES_NSGA2_H_

#include <list>
#include <vector>
#include "Chromosome.h"
#include "SearchStrategy.h"

using namespace metasolver; /* namespace clp */

class NSGA2 : public SearchStrategy{




	public:
		NSGA2(ChromosomeFactory* fac, ActionEvaluator* evl, double p_mut=0.3, double p_cross=0.9,
			double pop_size=50, int maxiter=100, bool trace=false) :
			SearchStrategy(evl), fac(fac), p_mut(p_mut), p_cross(p_cross), pop_size(pop_size),
			maxiter(maxiter), s0(NULL), trace(trace) { };

		virtual ~NSGA2() { };

		/**
		 * Performs an iteration of the strategy
		 * @returns true if the search strategy has not finished yet
		 */
		void next();

		virtual double run(State& s, double tl=99999.9, clock_t bt=clock());



		/**
		 * compute the objective values of the chromosomes
		 */
		template<class container>
		void evaluate(container& children);

		//TODO: ordenar y filtrar por dominancia y crowding distance
		/**
		 * The filtered chromosomes should be deleted
		 */


		void nondominated_sort_list(list<Chromosome*>&  G){
			G.sort(by_value1);
			G.unique(same_value1);


		}

		void nondominated_sort(vector<Chromosome*>&  G){
			sort(G.begin(), G.end(), by_value1);
		}

		void binary_tournament_selection(vector<Chromosome*>& selection);

		void single_selection(vector<Chromosome*>& selection);


	private:


		static bool by_value1(Chromosome* c1, Chromosome* c2){
			return c1->get_best_value()>c2->get_best_value();
		}

		static bool same_value1(Chromosome* c1, Chromosome* c2){
			if( fabs(c1->get_best_value()-c2->get_best_value())<0.001){
				delete c2;
				return true;
			}else return false;
		}



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

		/**
		 * Number of generations
		 */
		int maxiter;

		int iter;

		/**
		 * Factory of chromosomes
		 */
		ChromosomeFactory* fac;

		/**
		 * Population
		 */
		vector<Chromosome*> pop;

		/**
		 * Estado inicial (para evaluar cromosomas)
		 */
		State* s0;

		bool trace;
};

#endif /* PLUGINS_MOP_STRATEGIES_NSGA2_H_ */
