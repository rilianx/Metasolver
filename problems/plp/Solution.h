/*
 * Solution.h
 *
 * Soluciones (ubicacion de cada pieza) en el formato JSON de ESICUP y su verificacion
 */

#ifndef PLP_SOLUTION_H_
#define PLP_SOLUTION_H_

#include <string>
#include <vector>
#include "Instance.h"

namespace plp {

/**
 * Pieza ubicada: v' = R v + t, con v en las coordenadas de la pieza (caja envolvente en el origen)
 */
struct Placement {
	int type;
	Mat3 R;
	Vec3 t;
};

struct Report {
	bool feasible;
	double height;
	double density;      //volumen de las piezas / (W*D*height)
	double max_overlap;  //mayor profundidad de penetracion entre dos piezas
	double max_outside;  //mayor distancia de un vertice fuera del contenedor
	int n_placed;
	std::vector<std::string> errors;
};

class Solution {
public:

	Solution(const Instance& inst) : inst(inst) { }

	/** lee una solucion de ESICUP (las piezas se identifican por el nombre del archivo) */
	static Solution load(const std::string& file, const Instance& inst);

	void save(const std::string& file) const;

	/**
	 * Verifica que cada pieza este dentro del contenedor (z >= 0), que las piezas
	 * no se solapen (profundidad de penetracion mayor que eps) y que se ubiquen
	 * todas las piezas de la instancia (salvo con partial=true).
	 */
	Report verify(double eps=1e-6, bool partial=false) const;

	double height() const;

	const Instance& inst;
	std::vector<Placement> items;
};

std::ostream& operator<<(std::ostream& os, const Report& r);

} /* namespace plp */

#endif /* PLP_SOLUTION_H_ */
