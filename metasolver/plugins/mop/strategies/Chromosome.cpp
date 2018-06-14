
#include "Chromosome.h"

int Chromosome::mode=0;

Chromosome::Chromosome(ActionEvaluator* evl) : SearchStrategy(evl)  {

}


/*
 * TODO: Operador de mutacion
 * Mutates the chromosome
 */
Chromosome* Chromosome::mutate(double p){

  if(mode==1) p=sqrt(p);
	else if (mode==2) p=p*p;

	Chromosome* c_new = clone();

	int r;
  if (mode!=3)
	   r= p*(double)(size()-1);
	else
	   r = rand()%size();

	list<Gen*>::iterator it= c_new->genes.begin();

	advance(it,r);
	(*it)->mutate();

	return c_new;
}

Chromosome* Chromosome::crossover(Chromosome* c2, double p){
	if (mode!=3){
		cout << "error: crossover should be called with Chromosome::mode=3" << endl;
		exit(0);
	}

	Chromosome* child = clone();

	int r = rand()%size();

	std::list<Gen*> ::iterator it = child->genes.begin();
	std::advance (it,r);

	while(it!=child->genes.end()){
		delete *it;
		it=child->genes.erase(it);
	}

	std::list<Gen*> ::iterator it2 = c2->genes.begin();
	std::advance (it2,r);

	while(it2!=c2->genes.end()){
		child->genes.push_back((*it2)->clone());
		it2++;
	}

	return child;
}
