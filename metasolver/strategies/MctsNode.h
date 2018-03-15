/*
 * MctsNode.h
 *
 *  Created on: 03-01-2018
 *      Author: investigador
 */

#ifndef MCTSNODE_H_
#define MCTSNODE_H_

#include "../State.h"
#include "SearchStrategy.h"
#include <string>
#include <sstream>

namespace metasolver {

class State;

class Action;

class MctsNode {
private:
	//class members
	State* state;
	Action* action;
	MctsNode* parent;
	double value; //mean of the simulations
	double standart_deviation;
	double mcts_value;
	double stimated_sd;
	vector<MctsNode*> children;
	int depth;
	int num_visits;
	int id;

	MctsNode* add_child() {


		if(children.size()==0){
			//cout<<"adding child from front action"<<endl;
			Action* action = actions.front();
			MctsNode* child = new MctsNode(this, *action);
			child->state->transition(*action);
			//child->state->get_actions(child->actions);
			//get_best_actions(*child->state,child->actions,50);
			actions.pop_front();
			children.push_back(child);

			return child;
		}else{
			if(	actions.size()>10){
			//cout<<"adding child from random action, 10% of the best actions"<<endl;
			int index;
			int size=actions.size();

			index=actions.size()-rand()%(actions.size()/10)-1;
			std::list<Action*>::iterator it1;
			it1=actions.begin();
			advance(it1,index);
			Action* action = *it1;

			MctsNode* child = new MctsNode(this, *action);
			child->state->transition(*action);
			//child->state->get_actions(child->actions);
			actions.erase(it1);
			children.push_back(child);
			return child;
			}else{
				Action* action = actions.front();
				MctsNode* child = new MctsNode(this, *action);
				child->state->transition(*action);
				//child->state->get_actions(child->actions);
				actions.pop_front();
				children.push_back(child);

				return child;
			}
		}

	}
	void updateValue() {
		int size = simulations.size();
		double mean = 0;
		for (int i = 0; i < size; i++) {
			mean += simulations[i];
		}
		mean = mean / size;
		value = mean;
	}

public:
	list<Action*> actions;

	vector<double> simulations;
	MctsNode();

	double getPonderatedSd(){
		int size=get_num_simulations();
		if(size<3){
			return stimated_sd;
		}else{
			double alpha=0.5/(size-2);
			return alpha*stimated_sd+(1-alpha)*standart_deviation;
		}
	}

	MctsNode(MctsNode* _parent, Action& _action) {
		//cout<<"creando hijo con padre y accion"<<endl;
		parent = _parent;
		state = _parent->state->clone();
		value = 0;
		action = &_action;
		depth = parent->depth + 1;
		num_visits = 0;
		standart_deviation = 0;
		mcts_value = 0;
		stimated_sd=0;
		id=-1;
		//state->get_actions(actions);

	}
	MctsNode(State& s) {
		//este constructor se utiliza para la raiz del arbol
		//cout <<"le paso el estado"<<endl;
		state = &s;
		//cout <<"inicializo al padre como NULL"<<endl;
		parent = NULL;
		//cout <<"inicializo la accion que creo este nodo como NULL"<<endl;
		action = NULL;
		value = 0;
		depth = 0;
		num_visits = 0;
		mcts_value = 0;
		standart_deviation = 0;
		stimated_sd=0;
		//state->get_actions(actions);
		id=0;
	}
	MctsNode* getParent() {
		return parent;
	}
	MctsNode* getChildI(int i) {
		return children[i];
	}
	int getNumChild() {
		return children.size();
	}

	MctsNode* expand() {
		//return add_child(num_visits==2);
		//cout<<"expanding node"<<endl;
		return add_child();
	}

	bool isFullyExpanded() {

		return actions.size() == 0;
	}


	void updateParent(double val) {
		if(parent->getDepth()>0){//si el padre no es la raiz
			if(parent->getNumChild()>1){//si no es la primera simulacion //getNumSimulations()>0
				parent->simulations.push_back(val);

				int size = parent->simulations.size();
				double mean = 0;
				for (int i = 0; i < size; i++) {
					mean += parent->simulations[i];
				}
				mean = mean / size;
				parent->value = mean;
				parent->update_sd();
			}
		}else{//si el padre es la raiz
			parent->simulations.push_back(val);
			int size = parent->simulations.size();
			double mean = 0;
			for (int i = 0; i < size; i++) {
				mean += parent->simulations[i];
			}
			mean = mean / size;
			parent->value = mean;
			parent->update_sd();
		}
	}
	bool isTerminal() {

		return actions.size() == 0;
	}
	State* getState() {
		return state;
	}
	double getValue() {
		return value;
	}
	void setValue(double val) {
		value = val;
	}
	int getDepth() {
		return depth;
	}
	bool hasChildren() {
		return children.size() > 0;
	}
	void addSimul(double sim) {
		simulations.push_back(sim);
		updateValue();
		update_sd();
		updateParent(sim);
	}
	int getNumActions() {
		return actions.size();
	}
	void addVisit() {
		num_visits++;
	}
	int getNumVisits() {
		return num_visits;
	}
	void update_sd() {
		double sd = 0;
		int size = simulations.size();
		double mean = value;		//meean(simulations);
		for (int i = 0; i < size; i++) {
			sd += pow(simulations[i] - mean, 2);
		}
		sd = sd / size;
		sd = sqrt(sd);
		standart_deviation = sd;
	}

	double getMctsValue() {
		return mcts_value;
	}
	void setMctsValue(double val) {
		mcts_value = val;
	}

	double get_sd() {
		return standart_deviation;
	}

	double get_stimated_sd() {
		return stimated_sd;
	}

	void set_stimated_sd(double sd){
		stimated_sd=sd;
	}
	int get_num_simulations() {
		return simulations.size();
	}
	int getId(){
		return id;
	}
	void setId(int _id){
		id=_id;
	}
	double getFirstSimul(){
		return simulations.front();
	}

};

} /* namespace std */

#endif /* MCTSNODE_H_ */
