/*
 * SmallLP.h
 *
 * Programa lineal en 3 variables: min c.x  s.a.  a_i.x <= b_i,
 * resuelto con un metodo de conjunto activo a partir de un punto factible.
 * Todos los iterados son factibles: si se alcanza el limite de iteraciones,
 * el resultado es factible aunque no sea optimo.
 */

#ifndef PLP_SMALLLP_H_
#define PLP_SMALLLP_H_

#include <vector>
#include "Geometry.h"

namespace plp {

class SmallLP {
public:

	void clear() { A.clear(); B.clear(); }

	void add(const Vec3& a, double b) { A.push_back(a); B.push_back(b); }

	int size() const { return A.size(); }

	/**
	 * Minimiza c.x partiendo de x0 (factible). Retorna el ultimo iterado.
	 * optimal indica si se cumplieron las condiciones de optimalidad.
	 */
	Vec3 minimize(const Vec3& c, const Vec3& x0, bool* optimal=NULL, int maxit=200) const;

private:
	std::vector<Vec3> A;
	std::vector<double> B;
};

} /* namespace plp */

#endif /* PLP_SMALLLP_H_ */
