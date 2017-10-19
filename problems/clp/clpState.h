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

	virtual Action* clone() const{ return new clpAction(*this); cout << space.getVolume() << endl;}

	const Block& block;
	const Space space;

};

bool is_constructible(const clpState& s, const Block& b);

class clpState : public State {
public:

    static bool left;

    enum Format{BR, _1C};

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

	virtual State* create_neighbor(State* s0);

	friend clpState* new_state(string file, int instance, double min_fr, int max_bl, int f);

	virtual double get_value() const{
		return ((double) cont->getOccupiedVolume()/(double) cont->getVolume());
	}

	virtual double get_value2() const{
		//return 0.0;
		return cont->getTotalWeight() / weight_of_allboxes;
	}


	virtual void get_actions(list< Action* >& actions) const;

	/*
	* Rearranges the elements in the path pseudo-randomly
	*/
	virtual int shuffle_path();
	

	int get_n_valid_blocks() {return valid_blocks.size();}

	//member variables
	Block* cont;
	map<const BoxShape*, int> nb_left_boxes;
	list<const Block*> valid_blocks;

	virtual void print() {
		cont->MatLab_print();
	}

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

	/*
	 * calculate the number of supported blocks for each block
	 * @n_supports number of support blocks for each block
	 * @zero_support_aabb set of blocks with no supports
	 */
	void compute_supports(map<const AABB*,int>& n_supports, set<const AABB*>& zero_support_aabb);

	/*
	 * update the number of supported blocks after placing the block
	 */
	void update_supports(const AABB* block,
			map<const AABB*,int>& n_supports, set<const AABB*>& zero_support_aabb);
	//menor dimension de las cajas restantes
	Vector3 mindim;

	static double weight_of_allboxes;

};



clpState* new_state(string file, int instance, double min_fr=0.98, int max_bl=10000, int i=clpState::BR);

} /* namespace clp */

#endif /* CLPSTATE_H_ */
