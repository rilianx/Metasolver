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
	State* state;
	Action* action;
	MctsNode* parent;
	double value;//mean of the simulations
	double standart_deviation;
	vector< MctsNode* > children;
	list<Action*> actions;
	int depth;
	int num_visits;
	//functions
	//add a child node by an action
	MctsNode* add_child( bool many_times){
		if(!many_times){
		Action* action=actions.front();
		MctsNode* child= new MctsNode(this,*action);
		child->state->transition(*action);
		child->state->get_actions(child->actions);
		actions.pop_front();
		children.push_back(child);

		return child;
		}else{
			//cout<<"num_visits:"<<num_visits<<endl;
		int index=actions.size()%2;
		int i=0;
		Action* action;
		for (list<Action*>::iterator it=actions.begin(); it != actions.end(); ++it){
			if(i==index){
				action=	*it;
				actions.remove(*it);
				break;
			}
			i++;
		}
		MctsNode* child= new MctsNode(this,*action);
		child->state->transition(*action);
		child->state->get_actions(child->actions);
		actions.pop_front();
		children.push_back(child);

		return child;
		}
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
	vector<double> simulations;
	MctsNode();

	MctsNode(MctsNode* _parent,Action& _action){
		//cout<<"creando hijo con padre y accion"<<endl;
		parent=_parent;
		state=_parent->state->clone();
		value=0;
		action=&_action;
		depth=parent->depth+1;
		num_visits=0;
		standart_deviation=0;
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
		num_visits=0;
		standart_deviation=0;
		state->get_actions(actions);
	}
	MctsNode* getParent(){return parent;}
	MctsNode* getChildI(int i){return children[i];}
	int getNumChild(){return children.size();}

	MctsNode* expand(){
		return add_child(num_visits==2);
	}

	bool isFullyExpanded(){

		return actions.size()==0;
	}
	/*void update(double val){
		int size=simulations.size();
		double mean=0;
		for (int i = 0;i<size;i++)
		{
			mean+=simulations[i];
		}
		mean=mean/size;
		value=mean;
		if(parent)
		updateParent(val);
		value+=val;
	}*/

	void updateParent(double val){
		parent->simulations.push_back(val);

		int size=parent->simulations.size();
		double mean=0;
		for (int i = 0;i<size;i++)
		{
			mean+=simulations[i];
		}
		mean=mean/size;
		parent->value=mean;
	}
	bool isTerminal(){

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
		update_sd();
		updateParent(sim);
	}
	int getNumActions(){
		return actions.size();
	}
	void addVisit() {num_visits++;}
	int getNumVisits(){return num_visits;}
	void update_sd(){
		double sd=0;
		int size=simulations.size();
		double mean=meean(simulations);
		for(int i=0;i<size;i++){
			sd+=pow(simulations[i]-mean,2);
		}
		sd=sd/size;
		sd=sqrt(sd);
		standart_deviation=sd;
	}
	double meean(vector <double> numbers){
		double mean=0;
		int size=numbers.size();
		for(int i =0; i< size; i++)
			mean+=numbers[i];

		mean=mean/size;
		return mean;
	}
	double get_sd(){return standart_deviation;}
	int get_num_simulations(){return simulations.size();}
};

} /* namespace std */

#endif /* MCTSNODE_H_ */
