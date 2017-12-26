/*
 * NSGA2.cpp
 *
 *  Created on: 18 oct. 2017
 *      Author: iaraya
 */

#include "Gen.h"
#include "Chromosome.h"
#include "NSGA2.h"

//Basado: http://www.cleveralgorithms.com/nature-inspired/evolution/nsga.html
virtual list<Chromosome*> NSGA2::next(vector<Chromosome*>& pop){


	vector<Chromosome*> selected(pop_size);
	binary_tournament_selection(pop, selected);

	//reproduccion
	list<Chromosome*> children;
	Chromosome *p1, *p2;
	for(int i=0; i<selected.size();i++){
		p1=selected[i];
		int j = (i%0==0)?  i+1 : i-1;
		if(j == selected.size()-1) p2=selected[0];
		else p2=selected[j];


		//cruzamiento
		Chromosome* child = p1;
		if((double)rand()/(double)RAND_MAX < p_cross){
			child = p1->crossover(p2);
			children.push_back(child);
		}

		//mutacion
		if((double)rand()/(double)RAND_MAX < p_mut){
			children.push_back(child->mutate());
		}
	}

	children.insert(children.begin(), pop.begin(), pop.end());

	return filter_nondominated_sort(children);
}

void NSGA2::binary_tournament_selection(vector<Chromosome*>& pop, vector<Chromosome*>& selection){

	//asumo que la poblacion esta ordenada por frontera y luego distancia
	for(int i=0; i<pop_size; i++){
		int r1=rand()%pop_size;
		int r2=rand()%pop_size;
		selection[i]=pop[min(r1,r2)];
	}
}

void NSGA2::calculate_objectives(list<Chromosome*>& children){
	for(auto chrom : children){
		chrom->get_value();
		chrom->get_value2();
	}
}

void NSGA2::setP_mut(double p_mut){
	this->p_mut = p_mut;
}
void NSGA2::setP_cross(double p_cross){
	this->p_cross = p_cross;
}
void NSGA2::setPop_size(int pop_size){
	this->pop_size = pop_size;
}

double NSGA2::getP_mut(){
	return this->p_mut;
}
double NSGA2::getP_cross(){
	return this->p_cross;
}
int NSGA2::getPop_size(){
	return this->pop_size;
}
