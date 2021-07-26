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
	volume((x2-x1) * (y2-y1) * (z2-z1)), block(NULL), box(NULL) {}

AABB::AABB(const Vector3& mins, const Block* b) : mins(mins),
			maxs(mins+*b), block(b), volume(block->getVolume()) {}

AABB::AABB(const Vector3& mins, const Vector3& maxs) : mins(mins), maxs(maxs), block(NULL), box(NULL),
		volume( (maxs.getX() - mins.getX())*(maxs.getY() - mins.getY())*(maxs.getZ() - mins.getZ()) ) { };

long AABB::getOccupiedVolume() const {return block->getOccupiedVolume();}

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
//Calcula superficie de contacto entre 2 bloques AABB
double AABB::contact_surfaceZ(const AABB& aabb) const{
	double sc=0.0;
	for(auto unitary_aabb:*aabb.block->boxes){
		AABB un_aabb(unitary_aabb+aabb.getMins());
		if(getZmin() == un_aabb.getZmax())
			sc+=surface_in_contact(*this, un_aabb);
		
	}
	return sc;
}

//compute the surface in contact of this (unitary aabb), related to the aabb_list
double AABB::contact_surfaceZ(list<const AABB*>& aabb_list) const{
	double sc=0;
	for(auto aabb:aabb_list)
		sc+=contact_surfaceZ(*aabb);
	
	return sc;
	//exit(0);
}



bool greater_volume(const AABB& a, const AABB& b) { return a.getVolume() > b.getVolume() ; }
}


