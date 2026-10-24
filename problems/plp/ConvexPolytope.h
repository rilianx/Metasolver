/*
 * ConvexPolytope.h
 *
 * Politopo convexo en representacion V (vertices) y H (facetas),
 * y prueba de interseccion por ejes separadores (SAT)
 */

#ifndef PLP_CONVEXPOLYTOPE_H_
#define PLP_CONVEXPOLYTOPE_H_

#include <vector>
#include <stdexcept>
#include "Geometry.h"

namespace plp {

/**
 * Semiespacio n.x <= d (n unitario y hacia afuera)
 */
struct Plane {
	Vec3 n;
	double d;
	Plane(const Vec3& n, double d) : n(n), d(d) { }
};

class ConvexPolytope {
public:

	/**
	 * Envolvente convexa de los puntos (algoritmo incremental).
	 * Los puntos a distancia menor que eps*diametro se consideran coplanares.
	 * Lanza std::runtime_error si los puntos no generan un solido.
	 */
	/** edges: calcular las direcciones de las aristas (para penetration_depth; no se necesitan en los NFP) */
	static ConvexPolytope hull(const std::vector<Vec3>& points, double eps=1e-9, bool edges=true);

	/** v' = R v + t (R es una rotacion) */
	ConvexPolytope transformed(const Mat3& R, const Vec3& t) const;

	ConvexPolytope translated(const Vec3& t) const;

	/** true si p.x <= d + eps para todas las facetas */
	bool contains(const Vec3& p, double eps=0.0) const;

	const std::vector<Vec3>& get_vertices() const { return vertices; }
	const std::vector<Plane>& get_facets() const { return facets; }
	const std::vector<Vec3>& get_edge_dirs() const { return edge_dirs; }
	const Box3& get_bbox() const { return bbox; }
	double get_volume() const { return volume; }

private:

	ConvexPolytope() : volume(0) { }

	void update_bbox();

	std::vector<Vec3> vertices;

	//facetas (las triangulaciones coplanares se fusionan)
	std::vector<Plane> facets;

	//direcciones unitarias de las aristas (sin repetir, salvo el signo)
	std::vector<Vec3> edge_dirs;

	Box3 bbox;

	double volume;
};

/**
 * Profundidad de penetracion de A y B: la menor superposicion de sus proyecciones
 * sobre los ejes candidatos (normales de facetas y productos cruz de aristas).
 * Para politopos convexos es la menor traslacion que los separa.
 * Si es <= 0 los politopos no se intersectan (o solo se tocan).
 * Termina en cuanto encuentra un eje con superposicion <= stop.
 */
double penetration_depth(const ConvexPolytope& A, const ConvexPolytope& B, double stop=0.0, Vec3* axis=NULL);

} /* namespace plp */

#endif /* PLP_CONVEXPOLYTOPE_H_ */
