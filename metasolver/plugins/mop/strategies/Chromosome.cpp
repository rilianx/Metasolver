
#include "Chromosome.h"

Chromosome::Chromosome(ActionEvaluator* evl) : SearchStrategy(evl)  {

}


/*
 * TODO: Operador de mutacion
 * Mutates the chromosome
 */
Chromosome* Chromosome::mutate(double p){

	Chromosome* c_new = clone();

	int r = p*(double)(size());


	list<Gen*>::iterator it= c_new->genes.begin();

	advance(it,r);
	(*it)->mutate();
	it++;
	while(it!=c_new->genes.end()) {
		(*it)->mutate2();
		it++;
	}


	return c_new;
}

Chromosome* Chromosome::crossover(Chromosome* c2, double p){
	Chromosome* child = clone();

	int r = p*(double)(size());

	std::list<Gen*> ::iterator it = child->genes.begin();
	std::advance (it,r);

	//while(it!=child->genes.end()){
		delete *it;
		it=child->genes.erase(it);
	//}


	std::list<Gen*> ::iterator it2 = c2->genes.begin();
	std::advance (it2,r);

	//while(it2!=c2->genes.end()){
		child->genes.push_back((*it2)->clone());
		//it2++;
	//}

	return child;
}
