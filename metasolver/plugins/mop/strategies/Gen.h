/*
 * Gen.h
*/

#ifndef PLUGINS_MOP_STRATEGIES_GEN_H_
#define PLUGINS_MOP_STRATEGIES_GEN_H_

#include <list>
#include "SearchStrategy.h"

using namespace metasolver;

class Gen {

	private:
		int type; //type of box
		int n; //number of boxes
		int o; //orientation

	public:
		Gen::Gen();
		Gen::Gen(int type, int n, int o);

		void setType(int type);
		void setN(int n);
		void setO(int o);

		int getType();
		int getN();
		int getO();

};

#endif //PLUGINS_MOP_STRATEGIES_GEN_H_
