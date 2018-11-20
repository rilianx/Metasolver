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

	mclpState(const mclpState& S) : clpState(S) {

		for(Block* p:S.pallets){
			if(S.cont!=p)
				pallets.push_back(p->clone());
		}

		pallets.push_back(cont);
	}

	virtual ~mclpState(){
		for(Block* p:pallets)
			if(cont!=p) delete p;
	}


	virtual State* clone() const{
		State* st=new mclpState(*this);
		return st;
	}

	void new_pallet(){
		Block* b = new Block(cont->getL(), cont->getW(), cont->getH());
		pallets.push_back(b);
		cont=b;
	}


	friend mclpState* new_mstate(string file, int instance, double min_fr, int max_bl, bool rot, int nboxes);


	//member variables
	list<Block*> pallets;

protected:
	mclpState(Block* p) : clpState(p) {
		pallets.push_back(p);
	}


};



mclpState* new_mstate(string file, int instance, double min_fr=0.98, int max_bl=10000, bool rot=true, int nboxes=1);

} /* namespace clp */

#endif /* CLPSTATE_H_ */
