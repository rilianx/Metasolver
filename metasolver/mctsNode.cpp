/*
 * mctsNode.cpp
 *
 *  Created on: Mar 19, 2019
 *      Author: iaraya
 */

#include "mctsNode.h"
#include "State.h"
#include <stack>

namespace metasolver {

int mctsNode::count_nodes=0;

double mctsNode::B=0.0;
double mctsNode::C=0.0;
bool mctsNode::bp=false;

mctsNode::mctsNode(mctsNode* N, const Action* a) :
 parent(N), id(count_nodes++), action((a)? a->clone():NULL), var(0.0), mean(0.0),
 promise(0.0), nb_simulations(0), selected(false){

 }

 void mctsNode::calculate_promise(){
   if(children.size()<2) return;

   // se bonifica nodos con pocas simulaciones
   promise = mean + mctsNode::B*sqrt(var) + mctsNode::C*(1.0/sqrt(nb_simulations));

 }

State* mctsNode::get_state(const State* s){

	State* s0=s->clone();
	stack<const Action*> path;
	const mctsNode* aux=this;
	while(aux){
		if(aux->action) path.push(aux->action);
		aux=aux->parent;
	}

	while(!path.empty()){
		//cout << *path.top() << endl;
		s0->transition(*path.top());
		path.pop();
	}

	return s0;
}

} /* namespace metasolver */
