/*
 * Box.h
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

#include <stdio.h>
#include <list>

#include "Block.h"
#include "Vector3.h"
#include "Volume.h"

#ifndef AABB_H_
#define AABB_H_

using namespace std;

namespace clp {

class Block;


/**
 *
 */
class AABB {

	public:
		AABB(long x1, long y1, long z1, long x2, long y2, long z2) ;

		AABB(const Vector3& mins, const Block* b);

		AABB(const Vector3& mins, const Vector3& maxs);

		inline long getXmin() const {return mins.getX();}
		inline long getYmin() const {return mins.getY();}
		inline long getZmin() const {return mins.getZ();}

		inline long getXmax() const {return maxs.getX();}
		inline long getYmax() const {return maxs.getY();}
		inline long getZmax() const {return maxs.getZ();}

		inline long getL() const {return getXmax() - getXmin();}
		inline long getW() const {return getYmax() - getYmin();}
		inline long getH() const {return getZmax() - getZmin();}

		inline Volume getDimensions() const {return Volume(getL(),getW(),getH());}

		virtual long getOccupiedVolume() const;

		inline long getVolume() const {return volume;}

		const Block* getBlock() const {return block;}

		const Vector3& getMins() const { return mins;}

		const Vector3& getMaxs() const { return maxs;}

		inline long getSurface() const {return 2*(getL()*getW()+getL()*getH()+getW()*getH());}

        bool operator==(const AABB& b) const{
            return (volume == b.volume && mins==b.mins && maxs==b.maxs);
        }


    	bool operator <=(const AABB& s2) const{
            return (mins>=s2.mins && maxs<=s2.maxs);
    	}

    	bool operator >=(const AABB& s2) const{
            return (mins<=s2.mins && maxs>=s2.maxs);
    	}

		/**
		 * Returns a list with the new AABB generated
		 * by subtracting b to *this
		 */
		list<AABB> subtract(const AABB& b) const;

		bool strict_intersects(const AABB& b) const {
			  return !( getXmax()<=b.getXmin() || b.getXmax() <=getXmin() ||
					  getYmax()<=b.getYmin() || b.getYmax()<=getYmin() ||
					  getZmax()<=b.getZmin() || b.getZmax()<=getZmin() );
		}

		bool intersects(const AABB& b) const {
			  return !( getXmax()<b.getXmin() || b.getXmax() <getXmin() ||
					  getYmax()<b.getYmin() || b.getYmax()<getYmin() ||
					  getZmax()<b.getZmin() || b.getZmax()<getZmin() );
		}

		friend inline std::ostream& operator <<(std::ostream& os, const AABB& v);



	protected:
	    Vector3 mins;
	    Vector3 maxs;
	    const Block* block;

        long volume;

};

bool greater_volume(const AABB& a, const AABB& b) ;

inline std::ostream& operator <<(std::ostream& os, const AABB& ab){
	return os << ab.mins << "," << ab.maxs  ;
}

}
#endif /* AABB_H */
