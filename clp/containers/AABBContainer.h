/*
 * AABBContainer.h
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

#include "btBulletDynamicsCommon.h"
#include <BulletCollision/BroadphaseCollision/btDbvt.h>
#include <set>
#include <iostream>

#include "../objects2/AABB.h"
#include "AABBTree.h"

using namespace std;

#ifndef AABBContainer_H_
#define AABBContainer_H_

namespace clp {




template<class T>
class AABBContainer {

public:

	AABBContainer() : sz(0) {  };

	AABBContainer(const AABBContainer<T>& a) : sz(0) {
		cout << "AABBContainer copy should not be used" << endl;
	}


	virtual ~AABBContainer() { }

	void insert(const T& object){
		tree.insert(_insert(object));
		sz++;
	}

	void erase(const T& object){tree.erase(object); _erase(object);  sz--;}

	virtual list<const T*> get_intersected_objects(const AABB& volume) const {
		return tree.get_intersected_objects(volume);
	};

	virtual list<const T*> get_bisected_objects(long cut) const {
		return tree.get_bisected_objects(cut);
	};


	virtual void insert(list<T>& objects){
		typename list<T>::iterator it;

		for(it=objects.begin();it!=objects.end();it++)
			insert(*it);
	}

	/*
	 * Only used by AABBContainer<Space>
	 */
	virtual void crop_volume(const AABB& volume, const Vector3& cont, const Vector3& mindim) { };


    virtual const T& top() const=0;// { return *tree.top(); }

    virtual bool has_next() const=0;// { return false; }

    virtual const T& next() const= 0;

    virtual void pop(){ erase(top()); }

    virtual int size() const{ return sz;}

protected:
	AABBTree<T> tree;
	/**
	 * returns a reference to the inserted object
	 */
	virtual const T& _insert(const T& object)=0;
	virtual void _erase(const T& object)=0;// { };

private:

	int sz;
};







} /* namespace clp */

#endif /* AABBContainer_H_ */
