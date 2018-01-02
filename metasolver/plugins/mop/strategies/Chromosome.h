/*
 * Chromosome.h
 */

#ifndef PLUGINS_MOP_STRATEGIES_CHROMOSOME_H_
#define PLUGINS_MOP_STRATEGIES_CHROMOSOME_H_

#include <list>
#include "Gen.h"
#include "SearchStrategy.h"

using namespace metasolver;

class Chromosome : public SearchStrategy{

	/**
	 * TODO: (Ignacio) Calculate the objectives
	 */
public:

	Chromosome(ActionEvaluator* evl);

	virtual ~Chromosome(){ };

	virtual Chromosome* clone()=0;
	virtual Chromosome* mutate(double p=1.0);
	virtual Chromosome* crossover(Chromosome* c2, double p=1.0);

	virtual void write(std::ostream&) const=0;

	virtual int size() const { return genes.size(); }

	friend std::ostream& operator<<(std::ostream &strm, const Chromosome &a){ a.write(strm); return strm; }

	std::list<Gen*> genes;

};


class ChromosomeFactory{

public:

	virtual ~ChromosomeFactory(){ };

	virtual Chromosome* generate_random_chromosome(ActionEvaluator* evl, State& s)=0;


};

#endif //PLUGINS_MOP_STRATEGIES_CHROMOSOME_H_
