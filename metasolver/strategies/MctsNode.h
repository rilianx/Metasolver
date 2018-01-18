/*
 * MctsNode.h
 *
 *  Created on: 03-01-2018
 *      Author: investigador
 */

#ifndef MCTSNODE_H_
#define MCTSNODE_H_

#include "../State.h"
//#include "../MTCSestimator.h"

namespace metasolver {

class State;

class Action;

class MctsNode {
private:
	//class members
	vector<double> simulations;
	State* state;
	Action* action;
	MctsNode* parent;
	float value;
	vector< MctsNode* > children;
	list<Action*> actions;
	int depth;
	//functions
	//add a child node by an action
	MctsNode* add_child(Action& action){
		MctsNode* child= new MctsNode(this,action);
		child->state->transition(action);
		child->state->get_actions(child->actions);
		actions.pop_front();
		children.push_back(child);

		return child;
	}
	void updateValue(){
		int size = simulations.size();
		double mean=0;
		for (int i=0;i<size;i++){
			mean+=simulations[i];
		}
		mean= mean/size;
		value=mean;
	}
public:
	MctsNode();
	/*MctsNode( MctsNode* parent):
	state(state), parent(parent),value(0)
	{}*/
	MctsNode(MctsNode* _parent,Action& _action){
		//cout<<"creando hijo con padre y accion"<<endl;
		parent=_parent;
		state=_parent->state->clone();
		value=0;
		action=&_action;
		depth=parent->depth+1;
		//state->get_actions(actions);

	}
	MctsNode(State& s){
		//este constructor se utiliza para la raiz del arbol
		//cout <<"le paso el estado"<<endl;
		state=&s;
		//cout <<"inicializo al padre como NULL"<<endl;
		parent=NULL;
		//cout <<"inicializo la accion que creo este nodo como NULL"<<endl;
		action=NULL;
		value=0;
		depth=0;
		state->get_actions(actions);
	}
	MctsNode* getParent(){return parent;}
	MctsNode* getChildI(int i){return children[i];}
	int getNumChild(){return children.size();}
	MctsNode* expand(){
	    /*list <Action*> :: iterator it;
		it=actions.begin();
		advance(it,children.size());
		return add_child(**it);*/
		Action* ac=actions.front();
		return add_child(*ac);
	}
	bool isFullyExpanded(){
		//return children.empty() && children.size() == actions.size();
		//cout<<"cuantas acciones tiene:"<<actions.size()<<endl;
		return actions.size()==0;
	}
	void update(double val){value+=val;}
	bool isTerminal(){
		//list< Action* > actions;
		//state->get_actions(actions);
		//cout<<"cuantas acciones tiene:"<<actions.size()<<endl;
		return actions.size()==0;
	}
	State* getState(){
		return state;
	}
	double getValue(){
		return value;
	}
	void setValue(double val){
		value=val;
	}
	int getDepth(){
		return depth;
	}
	bool hasChildren(){
		return children.size()>0;
	}
	void addSimul(double sim){
		simulations.push_back(sim);
		updateValue();
	}
	int getNumActions(){
		return actions.size();
	}
	  bool operator<(const MctsNode& otro) const {
	    return value < otro.value;
	  }
};

} /* namespace std */

#endif /* MCTSNODE_H_ */
