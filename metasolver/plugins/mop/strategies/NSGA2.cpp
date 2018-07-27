/*
 * NSGA2.cpp
 *
 *  Created on: 18 oct. 2017
 *      Author: iaraya
 */

#include "Gen.h"
#include "Chromosome.h"
#include "NSGA2.h"



double NSGA2::run(State& s, double tl, clock_t bt){
	begin_time=bt;
	timelimit=tl;
	s0=&s;

	pop.resize(pop_size);


	for(int i=0; i<pop_size; i++)
		pop[i]=fac->generate_random_chromosome(evl,*s.clone());

	//evaluate(pop);
	nondominated_sort(pop);

	for(iter=0; iter<maxiter; iter++){
		if(trace){
			cout << iter << endl;
			for(int j=0;j<pop.size();j++)
				cout << *pop[j]  << endl;
		}
		next();

	}

	return get_best_value();
}

//Basado: http://www.cleveralgorithms.com/nature-inspired/evolution/nsga.html
void NSGA2::next(){

	vector<Chromosome*> selected(pop_size);
	binary_tournament_selection(selected);
	//single_selection(selected);

	//reproduccion
	list<Chromosome*> children;
	Chromosome *p1, *p2;
	for(int i=0; i<selected.size();i++){
		p1=selected[i];
		int j = (i%2==0)?  i+1 : i-1;
		if(j == selected.size()-1) p2=selected[0];
		else p2=selected[j];


		//cruzamiento
		Chromosome* child = p1;
		if((double)rand()/(double)RAND_MAX < p_cross){
			child = p1->crossover(p2, (double)iter/(double)maxiter);
			children.push_back(child);
		}

		//mutacion
		if((double)rand()/(double)RAND_MAX < p_mut){
			children.push_back(child->mutate((double)iter/(double)maxiter));
		}

	}

	evaluate(children);
	children.insert(children.begin(), pop.begin(), pop.end());

	nondominated_sort_list(children); //ordena la población
	for(int i=0; i<pop_size; i++){
		if(children.size()==0)
			pop[i]=pop[0]->clone();
		else{
			pop[i]=children.front();
			children.pop_front();
		}
	}

	for(auto ch:children) delete ch;
}

void NSGA2::binary_tournament_selection(vector<Chromosome*>& selection){

	//asumo que la poblacion esta ordenada por frontera y luego distancia
	for(int i=0; i<pop_size; i++){
		int r1=rand()%pop_size;
		int r2=rand()%pop_size;
		selection[i]=pop[min(r1,r2)];
	}
}

void NSGA2::single_selection(vector<Chromosome*>& selection){

	//asumo que la poblacion esta ordenada por frontera y luego distancia
	for(int i=0; i<pop_size; i++){
		selection[i]=pop[i];
	}
}

template<class container>
void NSGA2::evaluate(container& children){
	for(auto chrom : children){
		State* s = s0->clone();
		double value=chrom->run(*s, begin_time, timelimit);
		//best_state update
		if(value > get_best_value()){
			 if(best_state) delete best_state;
			 best_state = s->clone();
			 cout << "[NSGA2] new best_solution_found ("<< get_time() <<"): " << value << " "
					 << best_state->get_path().size() << " nodes" << " genes="<< chrom->genes.size() << endl;
		}
	}

}
