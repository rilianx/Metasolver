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
#include <vector>

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

    enum Format{BR, _1C, BRw};

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

	friend clpState* new_state(string file, int instance, double min_fr, int max_bl, Format f);

	virtual double get_value() const{
		return round(((double) cont->getOccupiedVolume()/(double) cont->getVolume())*10000.0)/10000.0;
	}

	virtual double get_value2() const{
		//return 0.0;
		return  round((cont->getTotalWeight() / weight_of_allboxes)*10000.0)/10000.0;
	}

	static bool height_sort(const Action* a1, const Action* a2){
		const clpAction* ca1 = dynamic_cast<const clpAction*> (a1);
		const clpAction* ca2 = dynamic_cast<const clpAction*> (a2);

		if(ca1->space.get_location(ca1->block).getZ() < ca2->space.get_location(ca2->block).getZ())
			return true;
		return false;


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




	static double weight_of_allboxes;

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




};

clpState* new_state(string file, int instance, double min_fr=0.98, int max_bl=10000, clpState::Format f=clpState::BR);

//For the ANN
class compactState {
public:
	//distribucion de volumenes en el contenedor
	vector< vector <double> > volume_distribution[3];

	//cantidad de cajas por rango de dimension (x,y,z)
	vector< int > size_histogram[3];

	compactState(const clpState& s) {
		int l=30, w=10, h=10;
		int histo_ranges = 20;
		double max_size=120;
		double range_sizeL = max_size/histo_ranges;
		double range_sizeW = max_size/histo_ranges;
		double range_sizeH = max_size/histo_ranges;

		volume_distribution[0].resize(l);
		for(int i=0; i<l; i++)	volume_distribution[0][i].resize(w);

		volume_distribution[1].resize(l);
		for(int i=0; i<l; i++)	volume_distribution[1][i].resize(h);

		volume_distribution[2].resize(w);
		for(int i=0; i<w; i++)	volume_distribution[2][i].resize(h);

		for(int i=0;i<3;i++)
			size_histogram[i].resize(histo_ranges);

		for(auto box:s.nb_left_boxes){
			if(box.second > 0){
				for(int o=0; o<6; o++){
					if(box.first->getL((BoxShape::Orientation) o)>0)
					  size_histogram[0][int(box.first->getL((BoxShape::Orientation) o)/range_sizeL+0.5)]+=box.second;
					if(box.first->getW((BoxShape::Orientation) o)>0)
					  size_histogram[1][int(box.first->getW((BoxShape::Orientation) o)/range_sizeW+0.5)]+=box.second;
					if(box.first->getH((BoxShape::Orientation) o)>0)
					  size_histogram[2][int(box.first->getH((BoxShape::Orientation) o)/range_sizeH+0.5)]+=box.second;
				}
			}
		}

		for(int i=0;i<3;i++){
		 for(auto histo:size_histogram[i])
			cout<< histo << " " ;
		 cout << endl;
		}
	}
};

} /* namespace clp */

#endif /* CLPSTATE_H_ */
