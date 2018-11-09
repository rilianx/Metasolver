/*
 * Space.h
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

#ifndef SPACE_H_
#define SPACE_H_

#include "AABB.h"

namespace clp{

class Space : public AABB {

public:


	Space(const AABB& object, const Vector3& cont) ;

	Space(const Vector3& vec) ;

	virtual ~Space();

	Vector3 get_location(const Vector3& block) const;

	inline int get_manhattan_distance() const {return manhattan_distance;}

	int get_manhattan_distance(const Vector3& cont, const bool* anchor) const;

	/**
	 * @return Returns true if sp1 is lexicographically lower than sp2,
	 * otherwise returns false
	 */
	friend bool lex_lower(const Space& sp1, const Space& sp2);


	void initialize(const Vector3& cont);

	const bool* get_anchor() const {return anchor;}

	Vector3 get_anchor_vector() const {
		long x,y,z;
		x= (anchor[0]==0)? getXmin():getXmax();
		y= (anchor[1]==0)? getYmin():getYmax();
		z= (anchor[2]==0)? getZmin():getZmax();

		return Vector3(x,y,z);
	}


	static bool bottom_up;


private:
	bool anchor[3];
	int manhattan_distance;

};




}
#endif /* SPACE_H_ */
