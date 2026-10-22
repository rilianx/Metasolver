/*
 * Geometry.h
 *
 * Vectores y matrices 3D para el problema de carga de poliedros (PLP)
 */

#ifndef PLP_GEOMETRY_H_
#define PLP_GEOMETRY_H_

#include <cmath>
#include <iostream>
#include <limits>

namespace plp {

struct Vec3 {
	double x, y, z;

	Vec3() : x(0), y(0), z(0) {}
	Vec3(double x, double y, double z) : x(x), y(y), z(z) {}

	double operator[](int i) const { return (i==0)? x : (i==1)? y : z; }
	double& operator[](int i) { return (i==0)? x : (i==1)? y : z; }

	Vec3 operator+(const Vec3& b) const { return Vec3(x+b.x, y+b.y, z+b.z); }
	Vec3 operator-(const Vec3& b) const { return Vec3(x-b.x, y-b.y, z-b.z); }
	Vec3 operator-() const { return Vec3(-x, -y, -z); }
	Vec3 operator*(double s) const { return Vec3(x*s, y*s, z*s); }
	Vec3 operator/(double s) const { return Vec3(x/s, y/s, z/s); }
	Vec3& operator+=(const Vec3& b) { x+=b.x; y+=b.y; z+=b.z; return *this; }

	double norm() const { return std::sqrt(x*x+y*y+z*z); }
};

inline double dot(const Vec3& a, const Vec3& b) { return a.x*b.x + a.y*b.y + a.z*b.z; }

inline Vec3 cross(const Vec3& a, const Vec3& b) {
	return Vec3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

inline Vec3 vmin(const Vec3& a, const Vec3& b) {
	return Vec3(std::min(a.x,b.x), std::min(a.y,b.y), std::min(a.z,b.z));
}

inline Vec3 vmax(const Vec3& a, const Vec3& b) {
	return Vec3(std::max(a.x,b.x), std::max(a.y,b.y), std::max(a.z,b.z));
}

inline std::ostream& operator<<(std::ostream& os, const Vec3& v) {
	return os << "(" << v.x << "," << v.y << "," << v.z << ")";
}

/**
 * Matriz 3x3 (convencion de vectores columna: v' = M v)
 */
struct Mat3 {
	double m[3][3];

	Mat3() { for(int i=0;i<3;i++) for(int j=0;j<3;j++) m[i][j]=(i==j)? 1.0:0.0; }

	Vec3 operator*(const Vec3& v) const {
		return Vec3(m[0][0]*v.x + m[0][1]*v.y + m[0][2]*v.z,
				    m[1][0]*v.x + m[1][1]*v.y + m[1][2]*v.z,
				    m[2][0]*v.x + m[2][1]*v.y + m[2][2]*v.z);
	}

	bool is_identity(double eps=1e-12) const {
		for(int i=0;i<3;i++) for(int j=0;j<3;j++)
			if(std::fabs(m[i][j] - ((i==j)? 1.0:0.0)) > eps) return false;
		return true;
	}

	/** M^T M = I y det(M) = 1 */
	bool is_rotation(double eps=1e-6) const {
		for(int i=0;i<3;i++) for(int j=0;j<3;j++){
			double s=0;
			for(int k=0;k<3;k++) s+=m[k][i]*m[k][j];
			if(std::fabs(s - ((i==j)? 1.0:0.0)) > eps) return false;
		}
		double det = m[0][0]*(m[1][1]*m[2][2]-m[1][2]*m[2][1])
				   - m[0][1]*(m[1][0]*m[2][2]-m[1][2]*m[2][0])
				   + m[0][2]*(m[1][0]*m[2][1]-m[1][1]*m[2][0]);
		return std::fabs(det-1.0) <= eps;
	}
};

/**
 * Caja alineada a los ejes [lo,hi]
 */
struct Box3 {
	Vec3 lo, hi;

	Box3() : lo( std::numeric_limits<double>::max(),  std::numeric_limits<double>::max(),  std::numeric_limits<double>::max()),
			 hi(-std::numeric_limits<double>::max(), -std::numeric_limits<double>::max(), -std::numeric_limits<double>::max()) {}

	void add(const Vec3& p) { lo=vmin(lo,p); hi=vmax(hi,p); }
	void add(const Box3& b) { lo=vmin(lo,b.lo); hi=vmax(hi,b.hi); }

	Vec3 extent() const { return hi-lo; }
	double volume() const { Vec3 e=extent(); return e.x*e.y*e.z; }

	/** interseccion de profundidad mayor a eps */
	bool overlaps(const Box3& b, double eps) const {
		return !( hi.x<=b.lo.x+eps || b.hi.x<=lo.x+eps ||
				  hi.y<=b.lo.y+eps || b.hi.y<=lo.y+eps ||
				  hi.z<=b.lo.z+eps || b.hi.z<=lo.z+eps );
	}
};

} /* namespace plp */

#endif /* PLP_GEOMETRY_H_ */
