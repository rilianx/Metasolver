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

    enum Format{BR, _1C, BRw, BRwp, BRpc};

    enum FormatP{NORMAL, ALL_ONE, WEIGHT};

	clpState(const clpState& S) : State(S),
	cont(S.cont->clone()), nb_left_boxes(S.nb_left_boxes),
	valid_blocks(S.valid_blocks), mindim(S.mindim), 
	singlebox_blocks(NULL){

		if(S.singlebox_blocks)
		   singlebox_blocks = new AABBList(*S.singlebox_blocks);

	}

	virtual ~clpState(){
		if(cont) delete cont;
	}


	virtual State* clone() const{
		State* st=new clpState(*this);
		return st;
	}

	virtual State* create_neighbor(State* s0);

	friend clpState* new_state(string file, int instance, double min_fr, int max_bl, Format f, FormatP fp);
	friend clpState* new_state(long L, long W, long H, double Wmax, map<BoxShape*, int>& boxes);

	
	
	//MOMENTO 5
	//Ser penalizada con algunas restricciones blandas
	virtual double get_value() const{
		return cont->getOccupiedVolume()/cont->getVolume();
	}

	virtual double get_value2() const{
		//return 0.0;
		//return cont->getTotalProfit();
		return  cont->getTotalProfit() / profit_of_allboxes;
	}

	static bool height_sort(const Action* a1, const Action* a2){
		const clpAction* ca1 = dynamic_cast<const clpAction*> (a1);
		const clpAction* ca2 = dynamic_cast<const clpAction*> (a2);

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
	Block* cont;
	map<const BoxShape*, int> nb_left_boxes;
	list<const Block*> valid_blocks;

	//For Practical Constraint operations
	AABBContainer<AABB>* singlebox_blocks;
	

	void get_singlebox_AABBs(list<const AABB*>& aabbs, const Block* this_b, Vector3 mins) const{
		clp::AABBContainer<clp::AABB> * blocks = this_b->blocks;
		if(blocks == NULL){
			aabbs.push_back(new AABB(mins, this_b));
			return;
		}else{
			const AABB* b=&blocks->top();
			while(true){
						get_singlebox_AABBs(aabbs, b->getBlock(), mins+b->getMins());
						if(blocks->has_next()) b=&blocks->next();
						else break;
			}
		}
	}

	static map<int,int> nb_boxes_by_type;
	static double weight_of_allboxes;
	static double profit_of_allboxes;
 	static double density_of_allboxes;
  	static double square_density_of_allboxes;
  	static int nb_boxes;
	static double Wmax;

	//practical constraints
	enum adj_type{DOWN=1, UP=2, BACK=4, FORTH=8, LEFT=16, RIGHT=32};
	void get_adjacent_aabbs(const AABB& ab, list<const AABB*>& aabb_list, int adj=clpState::UP, int d=10) const;
	map <int, set<string>> recursive_extra_movements(AABB aabb, int client, map <int, set<string>> extra_movementsMAP) const;
	double multidrop() const;
	double loadbalance() const;
	double loadbalanceA() const;
	double completeshipment() const;
	
	void general_block_generator(double min_fr, int max_bl, const Vector3& max_dim);

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



	/*
	 * calculate the number of supported blocks for each block
	 * @n_supports number of support blocks for each block
	 * @zero_support_aabb set of blocks with no supports
	 */
	void compute_supports(map<const AABB*,int>& n_supports, set<const AABB*>& zero_support_aabb);

	//correccion de error al ejecutar el make
	void get_actions(list< Action* >& actions) const;

	/*
	 * update the number of supported blocks after placing the block
	 */
	void update_supports(const AABB* block,
			map<const AABB*,int>& n_supports, set<const AABB*>& zero_support_aabb);
	//menor dimension de las cajas restantes
	Vector3 mindim;




};



clpState* new_state(string file, int instance, double min_fr=0.98, int max_bl=10000, clpState::Format f=clpState::BR,
		clpState::FormatP fp=clpState::NORMAL);

clpState* new_state(long L, long W, long H, double Wmax, map<BoxShape*, int>& boxes);

void read_instance(long& L, long& W, long& H, double& Wmax, map<BoxShape*, int>& boxes,
				 string file, int i, clpState::Format f=clpState::BR, clpState::FormatP fp=clpState::NORMAL);

} /* namespace clp */

#endif /* CLPSTATE_H_ */
