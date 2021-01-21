/*
 * clpState.h
 *
 *  Created on: 4 jul. 2017
 *      Author: iaraya
 */




#ifndef MCLPSTATE_H_
#define MCLPSTATE_H_

#include <map>
#include <string>
#include <vector>

#include "State.h"
#include "Block.h"


using namespace std;
using namespace metasolver;

namespace clp {

class BlockSet;
class mclpState;

class mclpAction : public Action{
public:
	mclpAction(const int id_pallet, const Block& block, const Space& space) : id_pallet(id_pallet), block(block), space(space) { }

	//mclpAction(const AABB& aabb, const Vector3& cont) : block(*aabb.getBlock()), space(aabb, cont) { }

	virtual Action* clone() const{ return new mclpAction(*this);}

	const int id_pallet;
	const Block& block;
	const Space space;

};

bool is_constructible(const mclpState& s, const Block& b);

class mclpState : public State {
public:

    static bool left;

	mclpState(const mclpState& S) : State(S),
	nb_left_boxes(S.nb_left_boxes),
	valid_blocks(S.valid_blocks){
		for(Block* p:S.pallets)
			pallets.insert(pallets.end(),p->clone());
	}

	virtual ~mclpState(){
		for(Block* p:pallets)
			delete p;
	}


	virtual State* clone() const{
		State* st=new mclpState(*this);
		return st;
	}

	virtual State* create_neighbor(State* s0);

	friend mclpState* new_mstate(string file, int instance, double min_fr, int max_bl);

	virtual double get_value() const{
		return pallets.size();
	}

	/*virtual double get_value2() const{
		return 0.0;
	}*/

	static bool height_sort(const Action* a1, const Action* a2){
		const mclpAction* ca1 = dynamic_cast<const mclpAction*> (a1);
		const mclpAction* ca2 = dynamic_cast<const mclpAction*> (a2);

		if(ca1->space.get_location(ca1->block).getZ() < ca2->space.get_location(ca2->block).getZ())
			return true;
		return false;
	}

	virtual void get_actions(list< Action* >& actions, int nsample=0) const;

	/*
	* Rearranges the elements in the path pseudo-randomly
	*/
	virtual int shuffle_path();


	int get_n_valid_blocks() {return valid_blocks.size();}

	//member variables
	vector<Block*> pallets;
	map<const BoxShape*, int> nb_left_boxes;
	list<const Block*> valid_blocks;

	virtual void print() {
		//cont->MatLab_print();
	}


protected:

	virtual void _transition(const Action& action);

private:

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

	void general_block_generator(double min_fr, int max_bl, const Vector3& max_dim);



};



mclpState* new_mstate(string file, int instance, double min_fr=0.98, int max_bl=10000);

} /* namespace clp */

#endif /* CLPSTATE_H_ */
