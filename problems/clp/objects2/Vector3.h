/*
 * Vector3.h
 *
 *  Created on: 02-06-2017
 *      Author: ignacio
 */

#ifndef OBJECTS_VECTOR3_H_
#define OBJECTS_VECTOR3_H_
#include <iostream>

namespace clp {

class Vector3 {

	public:

	Vector3() : x(0),y(0),z(0) { };

	Vector3(long x, long y, long z) : x(x),y(y),z(z) { };

	Vector3 min0() const{
		Vector3 v(std::max(getX(),(long)0),std::max(getY(),(long)0),std::max(getZ(),(long)0));
		return v;
	}

	long getX() const {return x;}
	long getY() const {return y;}
	long getZ() const {return z;}

	void setX(long xx) {x=xx;}
	void setY(long yy) {y=yy;}
	void setZ(long zz) {z=zz;}

	Vector3& operator-=(const Vector3& b) {
	   x-=b.x; y-=b.y; z-=b.z;
	   return *this;
	 }

	bool operator==(const Vector3& b) const{
       return (x==b.x && y==b.y && z==b.z);
    }

	Vector3& operator+=(const Vector3& b) {
	   x+=b.x; y+=b.y; z+=b.z;
	   return *this;
	 }

	bool operator >=(const Vector3& s2) const{
        return (x>=s2.x && y>=s2.y && z>=s2.z);
	}

	bool operator <=(const Vector3& s2) const{
        return (x<=s2.x && y<=s2.y && z<=s2.z);
	}

	//only for sorting use
	bool operator<(const Vector3& s2) const{
		return lex_lower(s2);
	}

	bool lex_lower(const Vector3& s2) const{
    	if(x!= s2.x) return (x < s2.x);
    	if(y!= s2.y) return (y < s2.y);
    	if(z!= s2.z) return (z < s2.z);
    	return false;
	}

	friend
	std::ostream& operator <<(std::ostream& os, const Vector3& v);

	private:

	long x;
	long y;
	long z;

};

inline Vector3 operator+(Vector3 lhs, const Vector3& rhs){
  lhs += rhs;
  return lhs;
}

inline Vector3 operator-(Vector3 lhs, const Vector3& rhs){
  lhs -= rhs;
  return lhs;
}

inline std::ostream& operator <<(std::ostream& os, const Vector3& v){
	return os << "(" << v.x << "," << v.y << "," <<v.z << ")" ;
}

} /* namespace clp */

#endif /* OBJECTS_VECTOR3_H_ */
