/*
 * State.h
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

#include <map>
#include <string>
#include <list>
#include <iostream>
#include <functional>




#ifndef STATE_H_
#define STATE_H_


#include "ActionEvaluator.h"
#include <algorithm>


using namespace std;
namespace clp{

class Action{
public:
	virtual Action* clone() const=0;

	virtual ~Action(){}

};



/**
 * Represent a partial solution of the problem. It consists in
 * the container (block) with the located blocks, a map of the left boxes and
 * a list of valid blocks which can be put in the container.
 */
class State {
public:

	State() : parent (NULL), root(false), evl(NULL) {}

	virtual State* copy(bool root=false) const = 0;

	virtual void set_evaluator(ActionEvaluator* _evl){
		evl=_evl;
	}

	virtual State* create_neighbor(State* s0){
		return NULL;
	}

	virtual ~State() {
		while(!path.empty()){
			delete path.front();
			path.pop_front();
		}
	}

	/**
	 * The value of the objective function
	 */
	virtual double get_value() const = 0;

	void transition(const Action& action) {
		path.push_back(action.clone());
		_transition(action);
	}
	
	/*
	* Rearranges the elements in the path randomly
	*/
	virtual int shuffle_path(){
	    cout << "why!!" << endl;
	    return 0;
	   // std::random_shuffle ( path.begin(), path.end() );
	}


	virtual Action* best_action(){
		list< Action* > actions;
		if(get_best_actions(actions,1)) return *actions.begin();
		else return NULL;
	}

	virtual int get_best_actions(list< Action* >& bactions, int n){
		if(!evl) {
			cout << "The function State::get_best_actions should be implemented or an "
					<< "ActionEvaluator should be provided" << endl;
			exit(0);
		}

		//the best n actions ranked from worst to best
		multimap<double,Action*> ranked_actions;
		//map<double,Action*> ranked_actions;

		list< Action* > actions;
		get_actions(actions);

		//if(actions.size()<=n) {bactions=actions; return bactions.size();}

		while(!actions.empty()){
			Action* a=actions.front(); actions.pop_front();
			double eval = evl->eval_action_rand(*this,*a);
			if(eval>0 && (ranked_actions.size()<n || ranked_actions.begin()->first < eval)){
				ranked_actions.insert(make_pair(eval,a));
				if(ranked_actions.size()==n+1) {
					delete (ranked_actions.begin()->second);
					ranked_actions.erase(ranked_actions.begin());
				}
			}else delete a;
		}

		//se colocan las acciones en la lista en el orden inverso
		while(ranked_actions.size()>0){
			//cout << "eval:" << ranked_actions.begin()->first << endl;
			bactions.push_front( ranked_actions.begin()->second );
			ranked_actions.erase(ranked_actions.begin());
		}

		return bactions.size();

	}

	virtual void get_actions(list< Action* >& actions) = 0;

	virtual Action* next_action(State& final){
	    if(get_path().size() >= final.get_path().size() ) return NULL; 
	    
	    list< const Action* >::iterator act=final.get_path().begin();
	    advance(act,get_path().size());
    	
    	return (*act)->clone();
	}

	bool is_root(){ return root; }

	list<const Action*>& get_path() const{ return path;}

	ActionEvaluator* get_evaluator(){ return evl; }

	virtual void print() {  }

protected:

	State(const State& S, bool root) : evl(S.evl), parent(&S), root(root){
		list<const Action*>::iterator it=S.get_path().begin();
		for(;it!=S.path.end();it++)
			path.push_back((*it)->clone());
	}


	virtual void _transition(const Action& action) = 0;

	const State* parent;
	bool root;

	//list of actions for reconstructing the state from scratch
	mutable list<const Action*> path;

	ActionEvaluator* evl;

};


class MO_State : public State {

public:
	/**
	 * The value of the second objective function
	 */
	virtual double get_value2() const = 0;

};


}

#endif /* STATE_H_ */
