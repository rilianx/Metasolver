/*
 * Vector3.h
 *
 *  Created on: 02-06-2017
 *      Author: ignacio
 */

#ifndef OBJECTS_VECTOR3_H_
#define OBJECTS_VECTOR3_H_
#include <iostream>
#include <math.h>

namespace clp {

class Vector3 {

	public:

	Vector3() : x(0),y(0),z(0) { };

	Vector3(double x, double y, double z) : x(x),y(y),z(z) { };

	long getX() const {return ceil(x);}
	long getY() const {return ceil(y);}
	long getZ() const {return ceil(z);}

	double getX_d() const {return x;}
	double getY_d() const {return y;}
	double getZ_d() const {return z;}

	void setX(double xx) {x=xx;}
	void setY(double yy) {y=yy;}
	void setZ(double zz) {z=zz;}

	Vector3& operator-=(const Vector3& b) {
	   x=getX()-b.getX(); y=getY()-b.getY(); z=getZ()-b.getZ();
	   return *this;
	 }
	 
	bool operator==(const Vector3& b) const{
       return (x==b.x && y==b.y && z==b.z);
    }

	Vector3& operator+=(const Vector3& b) {
	   x=getX()+b.getX(); y=getY()+b.getY(); z=getZ()+b.getZ();
	   return *this;
	 }

	bool operator >=(const Vector3& s2) const{
        return (getX()>=s2.getX() && getY()>=s2.getY() && getZ()>=s2.getZ());
	}

	bool operator <=(const Vector3& s2) const{
        return (getX()<=s2.getX() && getY()<=s2.getY() && getZ()<=s2.getZ());
	}

	//only for sorting use
	bool operator<(const Vector3& s2) const{
		return lex_lower(s2);
	}

	bool lex_lower(const Vector3& s2) const{
    	if(getX()!= s2.getX()) return (getX() < s2.getX());
    	if(getY()!= s2.getY()) return (getY() < s2.getY());
    	if(getZ()!= s2.getZ()) return (getZ() < s2.getZ());
    	return false;
	}

	friend
	std::ostream& operator <<(std::ostream& os, const Vector3& v);

	private:

	double x;
	double y;
	double z;

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
