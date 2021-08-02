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
	volume((x2-x1) * (y2-y1) * (z2-z1)), block(NULL), box(NULL),
	supported_weight(0.0), bottom_contact_surface(0.0) {}

AABB::AABB(const Vector3& mins, const Block* b) : mins(mins),
			maxs(mins+*b), block(b), volume(block->getVolume()),  
			supported_weight(0.0), bottom_contact_surface(0.0) {}

AABB::AABB(const Vector3& mins, const Vector3& maxs) : mins(mins), maxs(maxs), block(NULL), box(NULL),
		volume( (maxs.getX() - mins.getX())*(maxs.getY() - mins.getY())*(maxs.getZ() - mins.getZ()) ),  
		supported_weight(0.0), bottom_contact_surface(0.0) { };

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

/* Recursive function for propagating weights top-down */
void AABB::propagate_weight(double weight) const{
	for (auto supp:supporting_aabbs){
		// proporcion de la base en contacto con supp
		double cs = double(( min(supp->getXmax(),getXmax()) - max(supp->getXmin(),getXmin()) ) * 
										( min(supp->getYmax(),getYmax()) - max(supp->getYmin(),getYmin()) )) / bottom_contact_surface;
		
		double pweight =  weight*cs;
		supp->supported_weight += weight*cs;
		if(supp->supported_weight > supp->box->get_supported_weight()) {
			cout << *supp << endl;
			cout << supp->supported_weight <<">" << supp->box->get_supported_weight() << endl;
			exit(0);
		}

		supp->propagate_weight(pweight);
	}
}

void AABB::propagate_weight_const(double weight, map<const AABB*,double>& sup_weights) const{
	for (auto supp:supporting_aabbs){
		// proporcion de la base en contacto con supp
		double cs = double(( min(supp->getXmax(),getXmax()) - max(supp->getXmin(),getXmin()) ) * 
										( min(supp->getYmax(),getYmax()) - max(supp->getYmin(),getYmin()) )) / bottom_contact_surface;
		
		double pweight =  weight*cs;
		//cout << *supp << endl;
		//cout << supp->supported_weight<< endl;

		if(sup_weights.find(supp) == sup_weights.end())
			sup_weights[supp]=supp->supported_weight + weight*cs;
		else
			sup_weights[supp] += weight*cs;

		supp->propagate_weight_const(pweight, sup_weights);

	}
}



bool greater_volume(const AABB& a, const AABB& b) { return a.getVolume() > b.getVolume() ; }
}




