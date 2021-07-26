/*
 * Box.h
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

#include <stdio.h>
#include <list>

#include "BoxShape.h"
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

		enum dim{X,Y,Z};
		AABB get_face(int dim, bool min) const {
			Vector3 mins = this->mins;
			Vector3 maxs = this->maxs;

			if(dim==X){
				if(min){
					maxs.setX(mins.getX());
					mins.setX(mins.getX()-1);
				}else{
					mins.setX(maxs.getX());
					maxs.setX(mins.getX()+1);	
				}
			}

			if(dim==Y){
				if(min){
					maxs.setY(mins.getY());
					mins.setY(mins.getY()-1);
				}else{
					mins.setY(maxs.getY());
					maxs.setY(mins.getY()+1);	
				}
			}

			if(dim==Z){
				if(min){
					maxs.setZ(mins.getZ());
					mins.setZ(mins.getZ()-1);
				}else{
					mins.setZ(maxs.getZ());
					maxs.setZ(mins.getZ()+1);	
				}
			}

			return AABB(mins,maxs);
		}

		AABB& operator+=(const Vector3& b) {
			mins+=b; maxs+=b;
	   		return *this;
	 	}

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

		static long surface_in_contact(const AABB &b1, const AABB& b2){
			long l= min(b1.getXmax(),b2.getXmax()) - max(b1.getXmin(),b2.getXmin());
			long w= min(b1.getYmax(),b2.getYmax()) - max(b1.getYmin(),b2.getYmin()) ;
			if(l<=0 || w<=0) return 0;
			return l*w;
		}

		double contact_surfaceZ(list<const AABB*>& aabb_list) const;
		double contact_surfaceZ(const AABB& aabb) const;


		friend inline std::ostream& operator <<(std::ostream& os, const AABB& v);

		/* Para AABB boxes */
		
		//lista de AABB que soportan el AABB actual
		list<const AABB*> supporting_aabbs;
		double supported_weight;
		long bottom_contact_surface;

		void set_box(const BoxShape* b){ box=b; }



	protected:
	    Vector3 mins;
	    Vector3 maxs;
	    const Block* block;
		const BoxShape* box;

        long volume;

};

bool greater_volume(const AABB& a, const AABB& b) ;

inline std::ostream& operator <<(std::ostream& os, const AABB& ab){
	return os << ab.mins << "," << ab.maxs  ;
}

inline AABB operator+(AABB lhs, const Vector3& rhs){
	lhs += rhs;
	return lhs;
}

}
#endif /* AABB_H */
