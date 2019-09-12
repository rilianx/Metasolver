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

#include "clpState.h"
#include "State.h"
#include "Block.h"


using namespace std;
using namespace metasolver;

namespace clp {

class BlockSet;
class mclpState;

bool is_constructible(const mclpState& s, const Block& b);

class mclpState : public clpState {
public:

	mclpState(const mclpState& S) : clpState(S)/*, nb_left_boxes(S.nb_left_boxes), valid_blocks(S.valid_blocks) */{

	}

	virtual State* clone() const{
		State* st=new mclpState(*this);
		return st;
	}


	friend mclpState* new_mstate(string file, int instance, double min_fr, int max_bl, bool rot, int nboxes);

	double random(double l, double r) const{
		return l + ((double)rand()/RAND_MAX)*(r-l);
	}

	//Once a satisfactory solution has been constructed, priorities of used boxed are reduced
	void update_profits(map<const BoxShape*, int>* nb_inserted_boxes, map <int, int>& used_boxes) const{
		for(auto b:*nb_inserted_boxes)
			b.first->set_profit(b.first->getVolume()*pow(random(0.8, 1.0),used_boxes[b.first->get_id()]));
	}

	static map< int, BoxShape* > id2box;

protected:
	mclpState(Block* p) : clpState(p) {

	}


};



mclpState* new_mstate(string file, int instance, double min_fr=0.98, int max_bl=10000, bool rot=true, int nboxes=1);

} /* namespace clp */

#endif /* CLPSTATE_H_ */
