/*
 * Chromosome.h
 */

#ifndef PLUGINS_MOP_STRATEGIES_CHROMOSOME_H_
#define PLUGINS_MOP_STRATEGIES_CHROMOSOME_H_

#include <list>
#include "Gen.h"

using namespace metasolver;

class Chromosome {

private:
	std::list<Gen> genes;

	/**
	 * TODO: (Ignacio) Calculate the objectives
	 */
public:
	Chromosome(list<Gen>& genes);

	virtual ~Chromosome(){ };
	virtual double get_value();
	virtual double get_value2();

	Chromosome* copy();
	Chromosome* mutate();
	Chromosome* crossover(Chromosome* c2);

	void add_rnd_gene(Chromosome* c);
	void remove_rnd_gene(Chromosome* c);
	void change_rnd_gene(Chromosome* c);

};

#endif //PLUGINS_MOP_STRATEGIES_CHROMOSOME_H_
