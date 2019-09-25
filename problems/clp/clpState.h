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

#include "State.h"
#include "Block.h"


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


	int get_n_valid_blocks() const {return valid_blocks.size();}

	//member variables
	Block* cont;
	map<const BoxShape*, int> nb_left_boxes;
	list<const Block*> valid_blocks;

	virtual void print() {
		cont->MatLab_print();
	}

	//retorna la prediccion de la red a partir del estado
	virtual double ann_prediction();




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

  double volume;

	int nb_valid_blocks;

	vector<double> dataL, dataW, dataH;
	vector<double> spacesL, spacesW, spacesH;

	vector<double> volumes;
	vector<double> space_volumes;

	friend std::ostream& operator <<(std::ostream& os, const compactState& v);



	compactState(const clpState& s) {
		volume = s.get_value();
		nb_valid_blocks = s.get_n_valid_blocks();


		for(auto box:s.nb_left_boxes){
			if(box.second > 0){
				for(int j=0; j<box.second; j++) volumes.push_back((double)box.first->getVolume()/s.cont->getVolume());

				for(int o=0; o<6; o++){


					if(box.first->getL((BoxShape::Orientation) o)>0){
					  double x=(double)box.first->getL((BoxShape::Orientation) o)/s.cont->getL();
					  for(int j=0; j<box.second; j++) dataL.push_back(x);
					}

					if(box.first->getW((BoxShape::Orientation) o)>0){
						double x=(double)box.first->getW((BoxShape::Orientation) o)/s.cont->getW();
						for(int j=0; j<box.second; j++) dataW.push_back(x);
					}

					if(box.first->getH((BoxShape::Orientation) o)>0){
						double x=(double)box.first->getH((BoxShape::Orientation) o)/s.cont->getH();
						for(int j=0; j<box.second; j++) dataH.push_back(x);
					}
				}
			}
		}

		for(int i=0;i<s.cont->spaces->size();i++){
			const Space sp = (i==0)? s.cont->spaces->top() :  s.cont->spaces->next();
      spacesL.push_back((double)sp.getL()/s.cont->getL());
			spacesH.push_back((double)sp.getW()/s.cont->getW());
			spacesW.push_back((double)sp.getH()/s.cont->getH());
			space_volumes.push_back((double)sp.getVolume()/s.cont->getVolume());
		}


	}

};

inline std::ostream& operator <<(std::ostream& os, const compactState& v){
  os << v.volume<< endl;
	os << v.volumes.size() << endl; // nb_boxes
	os << v.space_volumes.size() << endl; // nb_spaces

	for(auto vol:v.volumes)
		 os << vol << " ";
	os << endl;

	for(auto data:v.dataL)
		 os << data << " ";

	os << endl;

	for(auto data:v.dataW)
		 os << data << " ";

	os << endl;

	for(auto data:v.dataH)
		 os << data << " ";

	os << endl;


	for(auto vol:v.space_volumes)
		 os << vol << " ";
	os << endl;

	for(auto data:v.spacesL)
		 os << data << " ";

	os << endl;

	for(auto data:v.spacesW)
		 os << data << " ";

	os << endl;

	for(auto data:v.spacesH)
		 os << data << " ";

	os << endl;

  //os << v.nb_valid_blocks<< endl;
  return os;
}

} /* namespace clp */

#endif /* CLPSTATE_H_ */
