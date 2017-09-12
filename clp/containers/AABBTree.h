/*
 * AABBTree.h
 *
 *  Created on: 23-06-2017
 *      Author: ignacio
 */

#include "btBulletDynamicsCommon.h"
#include <map>

#include "../objects2/AABB.h"
#include "../objects2/Space.h"

#ifndef AABBCONTAINER_AABBTREE_H_
#define AABBCONTAINER_AABBTREE_H_

using namespace std;

namespace clp {

class Space;

template<class T>
class btDbvtIntersector : public btDbvt::ICollide {

public:

  btDbvtIntersector(list<const T*>& intersected_objects) : intersected_objects(intersected_objects) {}

  virtual void 	Process (const btDbvtNode *, const btDbvtNode *){}
  virtual void 	Process (const btDbvtNode *n){
    intersected_objects.push_back( (const T*) (n->data));
  }

  virtual void 	Process (const btDbvtNode *n, btScalar s){}
  virtual bool 	Descent (const btDbvtNode *){return false;}
  virtual bool 	AllLeaves (const btDbvtNode *){return false;}

private:
  list<const T*>& intersected_objects;
};

btDbvtAabbMm get_btDbvtAabb(const AABB& a);

btDbvtAabbMm get_outer_btDbvtAabb(const AABB& a);

btDbvtAabbMm get_inner_btDbvtAabb(const AABB& a);

template<class T>
class AABBTree {
public:
	AABBTree() { };

	virtual ~AABBTree() { };

	virtual void insert(const T& object);

	virtual void erase(const T& object);

	virtual const T* top() const{
		obj2node_it=obj2node.begin();
		if(obj2node_it==obj2node.end()) return NULL;
		return obj2node_it->first;
	}

	virtual const T* next() const{
		obj2node_it++;
		if(obj2node_it==obj2node.end()) return NULL;
		return obj2node_it->first;
	}

	virtual list<const T*> get_intersected_objects(const AABB& volume) const ;

	list<const T*> get_bisected_objects(long cut) const;

private:
	mutable btDbvt _tree;
	map<const T*, btDbvtNode*> obj2node;
	mutable typename  map<const T*, btDbvtNode*>::const_iterator obj2node_it;
};



template<class T>
void AABBTree<T>::insert(const T& object){
	btDbvtNode * node = _tree.insert(get_btDbvtAabb(object), const_cast<T*> (&object));
    obj2node[&object] = node;
}

template<class T>
void AABBTree<T>::erase(const T& object){
	btDbvtNode * node = obj2node[&object];
	obj2node.erase(&object);
	_tree.remove(node);
}

template<class T>
list<const T*> AABBTree<T>::get_intersected_objects(const AABB& volume) const{
	list<const T*> intersected_objects;
	btDbvtIntersector<T> intersector(intersected_objects);
    _tree.collideTV ( _tree.m_root, get_outer_btDbvtAabb(volume), intersector);

    return intersected_objects;
}

template<class T>
list<const T*> AABBTree<T>::get_bisected_objects(long cut) const{
	list<const T*> intersected_objects;
	btDbvtIntersector<T> intersector(intersected_objects);

	AABB volume(cut,0,0,cut,1000,1000);

    _tree.collideTV ( _tree.m_root, get_outer_btDbvtAabb(volume), intersector);

    return intersected_objects;
}







} /* namespace clp */

#endif /* AABBCONTAINER_AABBTREE_H_ */
