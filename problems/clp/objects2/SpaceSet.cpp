/*
 * SpaceSet.cpp
 *
 *  Created on: 6 jul. 2017
 *      Author: iaraya
 */

#include "SpaceSet.h"

#include "Space.h"
#include "AABB.h"


using namespace std;
using namespace metasolver;

namespace clp{


void SpaceSet::crop_volume(const AABB& volume, const Vector3& cont, const Vector3& min_dim){
	list<const Space*> intersected_objects = get_intersected_objects(volume);


	list<Space> new_objects;
	//se eliminan todos los objetos que itersectan a volume
	while(!intersected_objects.empty()){
		const Space& obj = *intersected_objects.front();
		intersected_objects.pop_front();
		if(obj.strict_intersects(volume)){
			list<AABB> sub = obj.subtract(volume);
            while(sub.size()){
            	if(min_dim<=sub.front().getDimensions())
            		new_objects.push_back( Space(sub.front(), cont));
            	sub.pop_front();
            }
		}else
			new_objects.push_back(Space(obj));

		erase(obj);
	}


	//se filtran y se agregal los nuevos objetos
	remove_nonmaximal_objects(new_objects);
	AABBContainer::insert(new_objects);
}

void SpaceSet::remove_nonmaximal_objects(list<Space>& objs){
  objs.sort(greater_volume);

  typename list<Space>::iterator it=objs.begin();

  for(;it!=objs.end();it++){
	  typename list<Space>::iterator it2=it;
	  for(it2++;it2!=objs.end();it2++){
		  if((*it)>=(*it2)){
	         it2=objs.erase(it2);
	         it2--;
	      }
	  }
  }
}

const Space* SpaceSet::_insert(const Space& sp){
	pair<set<Space, by_manhattan_distance>::iterator,bool> p = data.insert(sp);
	if(global::TRACE) cout << "insert_space:+" <<  (*p.first) << ";" << &(*p.first) << endl;
    if(p.second)
    	return (&(*p.first));
    else
    	return NULL;
}

/*
const Space& SpaceSet::top() const{
	data_it = data.begin();
	const Space& sp=*data_it;
	data_it++;
	return sp;
}*/


const Space& SpaceSet::top() const{

	data_it = data.begin();
	marked=&(*data_it);
    data_it++;
	return *marked;
}

void SpaceSet::_erase(const Space& sp){
	if(global::TRACE) cout << "delete_space: " << sp << ";" << &sp << endl;
    data.erase(sp);
}

class SpaceSet ;
}




