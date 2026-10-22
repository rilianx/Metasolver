/*
 * LPBackend.h
 *
 * Interfaz minima para resolver programas lineales con una libreria externa
 * (ibex con SoPlex o CLP). Si el proyecto se compila sin ibex, create() retorna NULL.
 */

#ifndef PLP_LPBACKEND_H_
#define PLP_LPBACKEND_H_

#include <vector>
#include <utility>
#include <string>

namespace plp {

class LPBackend {
public:
	virtual ~LPBackend() { }

	/** nuevo problema con n variables (cotas (-inf,inf), costo 0, sin restricciones) */
	virtual void reset(int n) = 0;

	virtual void set_bounds(int var, double lo, double hi) = 0;

	virtual void set_cost(int var, double c) = 0;

	/** lo <= sum a_k x_k <= hi (filas dispersas) */
	virtual void add_row(const std::vector< std::pair<int,double> >& row, double lo, double hi) = 0;

	/** minimiza; retorna true si se encontro un optimo */
	virtual bool minimize() = 0;

	virtual std::vector<double> solution() const = 0;

	virtual std::string name() const = 0;

	/** backend disponible (NULL si se compilo sin ibex) */
	static LPBackend* create(double timeout=60.0);
};

} /* namespace plp */

#endif /* PLP_LPBACKEND_H_ */
