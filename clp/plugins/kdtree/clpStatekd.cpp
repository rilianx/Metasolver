/*
 * clpStatekd.cpp
 *
 *  Created on: 12 jul. 2017
 *      Author: iaraya
 */

#include "clpStatekd.h"
#include "BlockSet.h"

namespace clp {

BlockSet* clpState_kd::blocks=NULL;


void clpState_kd::get_actions(list< Action* >& actions, int n){

	list<const Block*>::iterator it;

    const Space* sp=NULL;

	while(cont.spaces->size()>0 && actions.size()==0){
		//cout << "spaces:" << cont.spaces->size() << endl;

	    sp=&cont.spaces->top();

	    list<const Block*> valid_blocks;

	    blocks->search_blocks(*this, *sp, valid_blocks, n);

	    //cout << valid_blocks.size() << endl;

		for(it = valid_blocks.begin();it!=valid_blocks.end();it++)
			 actions.push_back(new clpAction(**it,*sp));

		if(actions.size()==0) cont.spaces->pop();


	}

}


clpState_kd::clpState_kd(const clpState& S) : clpState(S,root)  {
	clpState_kd::blocks = new BlockSet(S.valid_blocks);
	valid_blocks.clear();
}

bool is_constructible(const clpState_kd& s, const Block& b){

	//if(s.is_tabu(b)) return false;

	if(is_constructible(*dynamic_cast<const clpState*>(&s), b))
		return true;

	//s.add_tabu(b);

	return false;

}

} /* namespace clp */
