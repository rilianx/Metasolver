/*
 * mctsNode.cpp
 *
 *  Created on: Mar 19, 2019
 *      Author: iaraya
 */

#include "mctsNode.h"
#include "State.h"
#include <stack>
#include <algorithm> //max



namespace metasolver {

int mctsNode::count_nodes=0;

double mctsNode::B=0.0;
double mctsNode::C=0.0;
bool mctsNode::bp=false;


mctsNode::mctsNode(mctsNode* parent, const Action* a, map<int, int>& level2nodes, map<int, int>& level2selectednodes) :
 parent(parent), id(count_nodes++), action((a)? a->clone():NULL), var(0.0), mean(0.0),
 promise(0.0), nb_simulations(0), selectable(false), sym(0), best(0), selected_count(0),
 depth(parent? (parent->depth+1):0), N(level2nodes[depth]),
 N_next(level2nodes[depth+1]), SN(level2selectednodes[depth]){

 }

 double mctsNode::calculate_promise(){
   if(children.size()<2) return 0.0;

   //BSG promise
   promise = 100000000 - 100000.0*N_next - 10000.0*depth -
   100.0*(std::max(SN-sqrt(N),0.0)*(selected_count==0)) -
   10.0*nb_simulations  + best;

   // se bonifica nodos con pocas simulaciones
   //promise = mean + mctsNode::B*sqrt(var) + mctsNode::C*(1.0/sqrt(nb_simulations));
   return promise;
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
