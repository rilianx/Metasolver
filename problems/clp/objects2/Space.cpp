/*
 * Space.cpp
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

#include "Space.h"

#include "Block.h"

namespace clp{

bool Space::bottom_up=false;

Space::Space(const Vector3& vec) : AABB(0, 0, 0, vec.getX(), vec.getY(), vec.getZ()) {
	initialize(vec);
}

Space::Space(const AABB& object, const Vector3& cont) : AABB(object) {
	// TODO Auto-generated constructor stub
	initialize(cont);
}

Space::~Space() {
	// TODO Auto-generated destructor stub
}

bool lex_lower(const Space & sp1, const Space & sp2){
	if(sp1.getXmin()!=sp2.getXmin()) return (sp1.getXmin()<sp2.getXmin());
	if(sp1.getYmin()!=sp2.getYmin()) return (sp1.getYmin()<sp2.getYmin());
	if(sp1.getZmin()!=sp2.getZmin()) return (sp1.getZmin()<sp2.getZmin());

	if(sp1.getW()!=sp2.getW()) return (sp1.getW()<sp2.getW());
	if(sp1.getL()!=sp2.getL()) return (sp1.getL()<sp2.getL());
	return (sp1.getH()<sp2.getH());
}

void Space::initialize(const Vector3& cont){
    int dist=0;
    if(getXmin() <= cont.getX() - getXmax() ){
    	dist+=getXmin() ;
      	anchor[0]=0;
    }else{
	 	dist+= cont.getX() - getXmax();
	   	anchor[0]=1;
    }


    if(getYmin()  <= cont.getY() - getYmax() ){
    	dist+=getYmin();
      	anchor[1]=0;
    }else{
    	dist+= cont.getY() - getYmax();
        anchor[1]=1;
    }

    if(Block::FSB || bottom_up || getZmin()  <= cont.getZ() - getZmax() ){
    	dist+=getZmin() ;
    	anchor[2]=0;
    }else{
		dist+= cont.getZ() - getZmax();
		anchor[2]=1;
    }

    manhattan_distance=dist;
}

int Space::get_manhattan_distance(const Vector3& cont, const bool* anchor) const{
    int dist=0;
    if(anchor[0]==0)
    	dist+=getXmin() ;
    else
	 	dist+= cont.getX() - getXmax();



    if(anchor[1]==0)
    	dist+=getYmin();
    else
    	dist+= cont.getY() - getYmax();

    if(anchor[2]==0)
    	dist+=getZmin() ;
    else
		dist+= cont.getZ() - getZmax();

    return dist;
}


Vector3 Space::get_location(const Vector3& block) const{
	long vec[3];
	if(!anchor[0])
		vec[0]=getXmin();
	else
		vec[0]=getXmax()-block.getX();

	if(!anchor[1])
		vec[1]=getYmin();
	else
		vec[1]=getYmax()-block.getY();

	if(!anchor[2])
		vec[2]=getZmin();
	else
		vec[2]=getZmax()-block.getZ();

	return Vector3(vec[0],vec[1],vec[2]);
}

bool by_manhattan_distance::operator()(const Space& sp1, const Space& sp2) {
	//if(&sp1==&sp2) return false;
    if(Space::bottom_up && sp1.getZmin() != sp2.getZmin()) return sp1.getZmin() < sp2.getZmin();

	if(sp1.get_manhattan_distance() != sp2.get_manhattan_distance())
		return sp1.get_manhattan_distance() < sp2.get_manhattan_distance();

	if(sp1.getVolume() != sp2.getVolume()) return (sp1.getVolume() > sp2.getVolume());

	return lex_lower(sp1,sp2);
/*
	if(sp1.getMins().lex_lower(sp2.getMins())) return true;
	if(sp2.getMins().lex_lower(sp1.getMins())) return false;

	return (sp1.getMaxs().lex_lower(sp2.getMaxs()));*/
}



}
