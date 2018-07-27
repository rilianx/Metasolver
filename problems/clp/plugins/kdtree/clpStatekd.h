/*
 * clpStatekd.h
 *
 *  Created on: 12 jul. 2017
 *      Author: iaraya
 */

#ifndef CLP_CLPSTATEKD_H_
#define CLP_CLPSTATEKD_H_

#include <unordered_set>
#include <set>
#include "../../clpState.h"

using namespace std;

namespace clp {

class clpState_kd : public clpState {
public:



	clpState_kd(const clpState_kd& S) : clpState(S) {


	}

	//first state
	clpState_kd(const clpState& S);

	virtual State* clone() const{
		State* st=new clpState_kd(*this);
		return st;
	}

/*
	bool is_tabu(const Block& b) const{
		if(nonvalid_blocks.find(&b)!=nonvalid_blocks.end()) return true;
		return false;
	}

	void add_tabu(const Block& b) const {
		nonvalid_blocks.insert(&b);
	}
*/

	//it is not required
	virtual void get_actions(list< Action* >& actions) const {
		cout << "clpState_kd::get_actions has not been implemented";
		exit(0);
	}




friend class Expander_kdtree;
private:
	static BlockSet* blocks;
	//mutable unordered_set <const Block*> nonvalid_blocks;
};

bool is_constructible(const clpState_kd& s, const Block& b);

} /* namespace clp */

#endif /* CLP_CLPSTATEKD_H_ */
