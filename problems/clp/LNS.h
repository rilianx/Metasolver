/*
 * LNS.h
 *
 * Busqueda de vecindario grande (LNS) sobre la mejor solucion del BSG.
 *
 * Como los espacios se seleccionan por cercania a las esquinas del contenedor,
 * el llenado avanza desde las esquinas hacia el centro, donde se encuentran los
 * frentes y queda el volumen perdido. Por eso la destruccion elimina los bloques
 * mas cercanos al centro del contenedor, y la reparacion vuelve a llenar el
 * espacio liberado con BSG desde la solucion parcial.
 */

#ifndef LNS_H_
#define LNS_H_

#include <random>
#include <vector>
#include "clpState.h"
#include "SearchStrategy.h"

namespace clp {

class clpLNS {
public:
	/**
	 * Distancia de un bloque al centro de la region destruida
	 */
	enum Shape{
		BOX,  //max(|dx|/L, |dy|/W, |dz|/H): region con forma de caja
		SLAB  //|dx|/L: rebanada central en x (ancho y alto completos)
	};

	/**
	 * @param s0 estado inicial (contenedor vacio)
	 * @param greedy greedy usado por el BSG de la reparacion
	 * @param beams numero de beams del BSG de la reparacion
	 * @param k0 proporcion inicial de bloques eliminados
	 * @param kmax proporcion maxima (crece cuando no hay mejora)
	 */
	clpLNS(const clpState& s0, ActionEvaluator* evl, SearchStrategy& greedy, Shape shape,
			int beams=4, double k0=0.1, double kmax=0.5, double growth=1.2, unsigned int seed=1) :
		s0(s0), evl(evl), greedy(greedy), shape(shape), beams(beams), k0(k0), kmax(kmax),
		growth(growth), rng(seed), iterations(0) { }

	/**
	 * Mejora la solucion s hasta el limite de tiempo tl (medido desde bt).
	 * Retorna la mejor solucion encontrada (nueva, el llamador la libera).
	 */
	clpState* run(const clpState& s, double tl, clock_t bt);

	int get_iterations() const { return iterations; }

	/**
	 * Construye el estado con los bloques indicados en sus posiciones
	 * (a partir del estado inicial s0)
	 */
	clpState* rebuild(const std::vector<PlacedBlock>& blocks) const;

private:

	/**
	 * Destruccion: retorna los bloques de s que se mantienen, eliminando
	 * los k mas cercanos al centro del contenedor (con un desplazamiento aleatorio)
	 */
	std::vector<PlacedBlock> destroy(const clpState& s, int k);

	const clpState& s0;
	ActionEvaluator* evl;
	SearchStrategy& greedy;
	Shape shape;
	int beams;
	double k0, kmax, growth;
	std::mt19937 rng;
	int iterations;
};

} /* namespace clp */

#endif /* LNS_H_ */
