/*
 * clp_chromosome.cpp
 *
 *  Created on: 27 dic. 2017
 *      Author: iaraya
 */

#include "VCS_Function.h"
#include "clp_chromosome.h"

namespace clp {


int GenCLP::max_rank=10;


Gen* GenCLP::clone(){
	return new GenCLP(*this);
}

void GenCLP::mutate(){

	rank=rand() % max_rank + 1;

}

void GenCLP::mutate2(){

	rank=rand() % (max_rank/10+1)+1;

}

Chromosome* ChromosomeCLP::clone() {
	return new ChromosomeCLP(*this);
}


ChromosomeCLP::~ChromosomeCLP() {
	// TODO Auto-generated destructor stub
	if(best_state)
		delete best_state;
}

Chromosome* ChromosomeFactoryCLP::generate_random_chromosome(ActionEvaluator* evl, State& s){
	ChromosomeCLP* c=new ChromosomeCLP(evl);
	c->initialize(&s);

	bool flag=true;
	while(flag){
		GenCLP* g= new GenCLP();

		VCS_Function* vcs_evl = dynamic_cast<VCS_Function*>(evl);

		list< Action* > actions;
		c->get_best_actions(s, actions, g->rank);


		if(actions.size()>0){
			Action* a= actions.back();
			s.transition(*a);

			g->rank=actions.size();
			c->genes.push_back(g);
		}else
			flag=false;

		//cout << "act:" << actions.size() << endl;

		for(auto action : actions ) delete action;


	}

	return c;
}

/**
 * Run the strategy
 */
double ChromosomeCLP::run(State& s, double tl, clock_t bt){
	begin_time=bt;
	timelimit=tl;
	initialize(&s);


	for(auto gen:genes){
		if(timelimit != 0.0 && get_time()>timelimit )
			return get_best_value() ;

		GenCLP* g=dynamic_cast<GenCLP*>(gen);

		VCS_Function* vcs_evl = dynamic_cast<VCS_Function*>(evl);

		list< Action* > actions;
		get_best_actions(s, actions, g->rank);


		if(actions.size()>0){
			Action* a= actions.back();
			s.transition(*a);
			g->rank=actions.size();
		}else{
			//the chromosome surpassed its maximum size
			while(gen!=genes.back()){
				delete genes.back();
				genes.pop_back();
			}
			delete genes.back();
			genes.pop_back();

			for(auto action : actions ) delete action;
			return get_best_value() ;
		}

		for(auto action : actions ) delete action;
	}


	bool flag=true;
	while(flag){
		GenCLP* g= new GenCLP();

		VCS_Function* vcs_evl = dynamic_cast<VCS_Function*>(evl);

		list< Action* > actions;
		get_best_actions(s, actions, g->rank);


		if(actions.size()>0){
			Action* a= actions.back();
			s.transition(*a);
			g->rank=actions.size();
			genes.push_back(g);
		}else
			flag=false;

		//cout << "act:" << actions.size() << endl;

		for(auto action : actions ) delete action;


	}

	return get_best_value() ;
}

void ChromosomeCLP::write(std::ostream &strm) const {
  for(auto gen:genes) strm << *gen << " ";
  strm << ":" << get_best_value();
}


void GenCLP::write(std::ostream &strm) const {
  strm << rank << " ";
}

};
