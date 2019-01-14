/*
 * SpaceSet.h
 *
 *  Created on: 23-06-2017
 *      Author: ignacio
 */

#include "AABB.h"
#include "Space.h"
#include "AABBContainer.h"
#include "GlobalVariables.h"

#ifndef SPACESET_H_
#define SPACESET_H_

using namespace std;
using namespace clp;

namespace clp {


struct by_manhattan_distance {
    bool operator()(const Space& sp1, const Space& sp2);
};


/**
 * Class for keeping free spaces of a Block
 */

//class AABBContainer;

class SpaceSet : public AABBContainer<Space> {
public:


	SpaceSet (Vector3& cont ) : cont(cont), marked(NULL) { }

	SpaceSet (const AABBContainer<Space>& a, Vector3& cont) : AABBContainer<Space>(), cont(cont), marked(NULL) {
		if(a.size()>0){
			const Space* obj=&a.top();
			while(true){
				insert(*obj);
				if(a.has_next()) obj=&a.next();
				else break;
			}
		}
	}


	virtual ~SpaceSet() { }

	virtual const Space& top() const;

    virtual bool has_next() const { return (data_it != data.end()); }

    /**
     * Removes the marked space (top)
     */
    virtual void pop(){ if(marked) erase(*marked); }


    virtual const Space& next() const {
		const Space& sp=*data_it; data_it++;
		if(&(*data_it) == marked) data_it++;
		return sp;
    };

    virtual void crop_volume(const AABB& volume, const Vector3& cont, const Vector3& min_dim);

	set<Space, by_manhattan_distance> data;

protected:
	virtual const Space* _insert(const Space& sp);

	virtual void _erase(const Space& sp);

	void remove_nonmaximal_objects(list<Space>& objs);


	mutable set<Space, by_manhattan_distance>::const_iterator data_it;


	Vector3 cont;
	//this vairable points the spaces returned by the last top
	mutable const Space* marked;
};




} /* namespace clp */

#endif /* SPACESET_H_ */
