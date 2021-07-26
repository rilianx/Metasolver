/*
 * AABBTree.cpp
 *
 *  Created on: 23-06-2017
 *      Author: ignacio
 */

#include "AABBTree.h"

namespace clp {



btDbvtAabbMm get_btDbvtAabb(const AABB& a){
return
	btDbvtAabbMm::FromMM (btVector3 ((btScalar) a.getXmin(),
			(btScalar) a.getYmin(),(btScalar) a.getZmin()),
			btVector3 ((btScalar) a.getXmax(), (btScalar) a.getYmax(),
			(btScalar) a.getZmax()));
}

btDbvtAabbMm get_outer_btDbvtAabb(const AABB& a){
	return
		btDbvtAabbMm::FromMM (btVector3 ((btScalar) a.getXmin()-0.2,
				(btScalar)a.getYmin()-0.2,(btScalar)a.getZmin()-0.2),
				btVector3 ((btScalar) a.getXmax()+0.2, (btScalar) a.getYmax()+0.2,
				(btScalar) a.getZmax()+0.2));
}

btDbvtAabbMm get_inner_btDbvtAabb(const AABB& a){
	return
		btDbvtAabbMm::FromMM (btVector3 ((btScalar) a.getXmin()+0.2,
				(btScalar)a.getYmin()+0.2,(btScalar)a.getZmin()+0.2),
				btVector3 ((btScalar) a.getXmax()-0.2, (btScalar) a.getYmax()-0.2,
				(btScalar) a.getZmax()-0.2));
}

template class AABBTree<Space> ;
template class AABBTree<AABB> ;

} /* namespace clp */
