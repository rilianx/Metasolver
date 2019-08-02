/*
 * AABB.cpp
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

#include "AABB.h"

using namespace std;

namespace clp {

AABB::AABB(long x1, long y1, long z1, long x2, long y2, long z2) : mins(x1,y1,z1), maxs(x2,y2,z2),
	volume((x2-x1) * (y2-y1) * (z2-z1)), block(NULL) {}

AABB::AABB(const Vector3& mins, const Block* b) : mins(mins),
			maxs(mins+*b), block(b), volume(block->getVolume()) {}

AABB::AABB(const Vector3& mins, const Vector3& maxs) : mins(mins), maxs(maxs), block(NULL),
		volume( (maxs.getX() - mins.getX())*(maxs.getY() - mins.getY())*(maxs.getZ() - mins.getZ()) ) { };

long AABB::getOccupiedVolume() const {return block->getOccupiedVolume();}

long AABB::volume_intersection (const AABB& aabb) const{
	 if (aabb.block!=NULL) return -1.0;
	 long volume_inter=0;
	 long xmin = max(getXmin(),aabb.getXmin());
	 long xmax = min(getXmax(),aabb.getXmax());
	 long ymin = max(getYmin(),aabb.getYmin());
	 long ymax = min(getYmax(),aabb.getYmax());
	 long zmin = max(getZmin(),aabb.getZmin());
	 long zmax = min(getZmax(),aabb.getZmax());
	 if(xmin>=xmax || ymin>=ymax || zmin>=zmax) return 0;

	 if(block->getOccupiedVolume() != block->getVolume()){
		 AABB aabb2 ((aabb.getMins() - getMins()).min0(), (aabb.getMaxs() - getMins()).min0());
		 list<const clp::AABB *> inter_ab = block->blocks->get_intersected_objects(aabb2);
		 for(auto ab:inter_ab)
			volume_inter+=ab->volume_intersection(aabb2);

	 }else
			 volume_inter=(xmax-xmin)*(ymax-ymin)*(zmax-zmin);

	 return volume_inter;
}

list<AABB> AABB::subtract(const AABB& b) const{
	const AABB& b1=*this;
	const AABB& b2=b;

	list<AABB> sub;

    if(b2.getXmax()<b1.getXmax())
    	sub.push_back( AABB(b2.getXmax(),b1.getYmin(),b1.getZmin(),b1.getXmax(),b1.getYmax(),b1.getZmax()));

    if(b2.getYmax()<b1.getYmax())
    	sub.push_back( AABB(b1.getXmin(),b2.getYmax(),b1.getZmin(),b1.getXmax(),b1.getYmax(),b1.getZmax()));

    if(b2.getZmax()<b1.getZmax())
    	sub.push_back( AABB(b1.getXmin(),b1.getYmin(),b2.getZmax(),b1.getXmax(),b1.getYmax(),b1.getZmax()));


    if(b2.getXmin()>b1.getXmin())
    	sub.push_back( AABB(b1.getXmin(),b1.getYmin(),b1.getZmin(),b2.getXmin(),b1.getYmax(),b1.getZmax()));

    if(b2.getYmin()>b1.getYmin())
    	sub.push_back( AABB(b1.getXmin(),b1.getYmin(),b1.getZmin(),b1.getXmax(),b2.getYmin(),b1.getZmax()));

    if(b2.getZmin()>b1.getZmin())
    	sub.push_back( AABB(b1.getXmin(),b1.getYmin(),b1.getZmin(),b1.getXmax(),b1.getYmax(),b2.getZmin()));

    return sub;
}



bool greater_volume(const AABB& a, const AABB& b) { return a.getVolume() > b.getVolume() ; }
}
