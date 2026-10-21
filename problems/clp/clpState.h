/*
 * clpState.h
 *
 *  Created on: 4 jul. 2017
 *      Author: iaraya
 */




#ifndef CLPSTATE_H_
#define CLPSTATE_H_

#include <map>
#include <string>

#include "../metasolver/State.h"
#include "objects2/Block.h"


using namespace std;
using namespace metasolver;

namespace clp {

class BlockSet;
class clpState;

class clpAction : public Action{
public:
	clpAction(const Block& block, const Space& space) : block(block), space(space) { }

	clpAction(const AABB& aabb, const Vector3& cont) : block(*aabb.getBlock()), space(aabb, cont) { }

	virtual Action* clone() const{ return new clpAction(*this); }

	const Block& block;
	const Space space;

};

bool is_constructible(const clpState& s, const Block& b);

class clpState : public State {
public:

	clpState(const clpState& S) : State(S),
	cont(S.cont->clone()), nb_left_boxes(S.nb_left_boxes),
	valid_blocks(S.valid_blocks), mindim(S.mindim){

	}

	virtual ~clpState(){
		if(cont) delete cont;
	}


	virtual State* clone() const{
		State* st=new clpState(*this);
		return st;
	}

	friend clpState* new_state(string file, int instance, double min_fr, int max_bl);

	virtual double get_value() const{
		return cont->getOccupiedVolume()/cont->getVolume();
	}

	virtual void get_actions(list< Action* >& actions) const;

	int get_n_valid_blocks() {return valid_blocks.size();}

	//member variables
	Block* cont;
	map<const BoxShape*, int> nb_left_boxes;
	list<const Block*> valid_blocks;

protected:

	virtual void _transition(const Action& action);

private:

	clpState(Block* cont) :  cont(cont), mindim(cont->getL(),cont->getW(),cont->getH()) {  };

	/**
	 * Remove the free spaces in the container that cannot
	 * include any leaving box
	 */
	void update_free_spaces() { };

	/**
	 * Remove blocks from the valid_block list that do not
	 * fit in any free space or cannot be constructed with the leaving boxes
	 */
	void update_valid_blocks() {
		list<const Block*>::iterator it = valid_blocks.begin();
		while(it!=valid_blocks.end()){
			if(!is_constructible(*this,**it))
				it=valid_blocks.erase(it);
			else it++;
		}

	};

	void update_min_dim();

	void general_block_generator(double min_fr, int max_bl, const Vector3& max_dim);

	//menor dimension de las cajas restantes
	Vector3 mindim;




};



clpState* new_state(string file, int instance, double min_fr=0.98, int max_bl=10000);

} /* namespace clp */

#endif /* CLPSTATE_H_ */
