/*
 * AABBList.h
 *
 *  Created on: 23-06-2017
 *      Author: ignacio
 */


#ifndef AABBCONTAINER_AABBLIST_H_
#define AABBCONTAINER_AABBLIST_H_

using namespace std;


namespace clp {

/**
 * Class for keeping free the AABB objects of a block
 */

class AABBList : public AABBContainer<AABB> {
public:

	AABBList ( ) { }

	AABBList (const AABBContainer<AABB>& a) : AABBContainer<AABB>() {
		if(a.size()>0){
			const AABB* obj=&a.top();
			while(true){
				insert(*obj);
				if(a.has_next()) obj=&a.next();
				else break;
			}
		}
	}

	virtual const AABB* _insert(const AABB& object){
		data.push_back(object);
		//cout << "insert_box:" <<  data.back() << endl;
		return &data.back();
	}

	virtual void _erase(const AABB& object){
		data.remove(object);
	}

	virtual const AABB& top() const{
		data_it = data.begin();
		const AABB& sp=*data_it; data_it++;
		return sp;
	}

    virtual bool has_next() const{ return (data_it != data.end()); }

    virtual const AABB& next() const{
    	const AABB& sp=*data_it; data_it++;
		return sp;
    };

	list<AABB>::iterator begin() { return data.begin(); }
    list<AABB>::iterator end()   { return data.end(); } 

private:
	list<AABB> data;
	mutable list<AABB>::const_iterator data_it;
};


} /* namespace clp */

#endif /* AABBCONTAINER_AABBLIST_H_ */
