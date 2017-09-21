/*
 * MO_BoxShape.h
 *
 *  Created on: 21 sept. 2017
 *      Author: iaraya
 */

#ifndef CLP_PLUGINS_MOP_MO_BOXSHAPE_H_
#define CLP_PLUGINS_MOP_MO_BOXSHAPE_H_

#include "../../objects2/BoxShape.h"

namespace clp {

class MO_BoxShape : public BoxShape {
public:

	MO_BoxShape(int id, long l, long w, long h, bool rot1=true, bool rot2=true, bool rot3=true, double weight=1.0) :
		BoxShape(id, l, w, h, rot1, rot2, rot3), weight(weight) { }

	double get_weight() const { return weight;}

private:
	double weight;
};

}


#endif /* CLP_PLUGINS_MOP_MO_BOXSHAPE_H_ */
