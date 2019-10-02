/*
 * Box.h
 *
 *  Created on: 2 jun. 2017
 *      Author: iaraya
 */

#include "Volume.h"

#ifndef OBJECTS_BOXSHAPE_H_
#define OBJECTS_BOXSHAPE_H_

namespace clp {

class BoxShape : public Volume {
public:
	enum Orientation{LWH=0, LHW, WLH, WHL, HLW, HWL, OR};


	BoxShape(int id, double l, double w, double h, bool rot1=true, bool rot2=true, bool rot3=true, double weight=1.0, double profit=1.0);

	bool is_valid(Orientation o) const {return is_valid_orientation[o];}

	long getL(Orientation o) const;
	long getW(Orientation o) const;
	long getH(Orientation o) const;

	double getL_d(Orientation o) const;
	double getW_d(Orientation o) const;
	double getH_d(Orientation o) const;

	/**
	 * returns the i-th dimension of the box oriented by o
	 */
	long get(int i, Orientation o) const;



    bool operator<(const BoxShape& bt) const{
    	return (id<bt.id);
    }

    inline int get_id() const {return id;}

    double get_weight() const { return weight; }

    double get_profit() const { return profit; }


private:
	int id;
	bool is_valid_orientation[6];

	double weight;
	double profit;

};

} /* namespace clp */

#endif /* OBJECTS_BOXSHAPE_H_ */
