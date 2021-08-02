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


	BoxShape(int id, double l, double w, double h, bool rot1=true, bool rot2=true, bool rot3=true, 
	double weight=1.0, double profit=1.0,
	//practical constraints
	int type=0, double supported_weight=0.0, double h_stability = 0.0, double v_stability = 0.0, int id_client=0);

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

	int get_type() const {return type;}
	double get_supported_weight() const{return supported_weight;}
	double get_h_stability() const {return h_stability;}
	double get_v_stability() const {return v_stability;}
	double get_id_client() const {return id_client;}

private:
	int id;
	bool is_valid_orientation[6];
	double weight;
	double profit;

	//Practical Constraints
	int id_client;
	int type;
	double supported_weight;
	double h_stability;
	double v_stability;

};

} /* namespace clp */

#endif /* OBJECTS_BOXSHAPE_H_ */
