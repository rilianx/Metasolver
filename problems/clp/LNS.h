/*
 * LNS.h
 *
 * Busqueda de vecindario grande (LNS) sobre la mejor solucion del BSG.
 *
 * Como los espacios se seleccionan por cercania a las esquinas del contenedor,
 * el llenado avanza desde las esquinas hacia el centro, donde se encuentran los
 * frentes y queda el volumen perdido. Por eso la destruccion elimina los bloques
 * mas cercanos a un espacio vacio (o al centro del contenedor), y la reparacion
 * vuelve a llenar el espacio liberado con BSG o en forma exacta con CP-SAT.
 */

#ifndef LNS_H_
#define LNS_H_

#include <random>
#include <vector>
#include <map>
#include <tuple>
#include "clpState.h"
#include "SearchStrategy.h"
#include "CPSAT.h"

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
	 * Centro de la region destruida
	 */
	enum Center{
		VOID,   //centro de un espacio libre (elegido con probabilidad proporcional a su volumen)
		MIDDLE  //centro del contenedor (con un desplazamiento aleatorio)
	};

	/**
	 * Reparacion
	 */
	enum Repair{
		BSG_REPAIR,   //BSG desde la solucion parcial
		EXACT_REPAIR, //CP-SAT con las cajas disponibles en la caja envolvente de los bloques eliminados
		HYBRID_REPAIR //BSG y, si no mejora, CP-SAT
	};

	/** parametros de la reparacion exacta */
	struct ExactParams {
		ExactParams() : max_time(1.0), max_boxes(40), workers(1), redundant(true) { }
		double max_time; //tiempo maximo por llamada a CP-SAT
		int max_boxes;   //maximo de cajas candidatas en el modelo
		int workers;     //hilos de CP-SAT
		bool redundant;  //restricciones redundantes (cumulative por eje y volumen)
	};

	/**
	 * @param s0 estado inicial (contenedor vacio)
	 * @param greedy greedy usado por el BSG de la reparacion
	 * @param solver solver CP-SAT (solo para la reparacion exacta o hibrida)
	 * @param beams numero de beams del BSG de la reparacion
	 * @param samples destrucciones evaluadas por iteracion (se elige la de mayor potencial de mejora)
	 * @param k0 proporcion inicial de bloques eliminados
	 * @param kmax proporcion maxima (crece cuando no hay mejora)
	 */
	clpLNS(const clpState& s0, ActionEvaluator* evl, SearchStrategy& greedy, Shape shape, Center center=VOID,
			Repair repair=BSG_REPAIR, CpSolver* solver=NULL, ExactParams exact=ExactParams(),
			int beams=4, int samples=8, double k0=0.1, double kmax=0.5, double growth=1.2, unsigned int seed=1);

	/**
	 * Mejora la solucion s hasta el limite de tiempo tl (medido desde bt).
	 * Retorna la mejor solucion encontrada (nueva, el llamador la libera).
	 */
	clpState* run(const clpState& s, double tl, clock_t bt);

	int get_iterations() const { return iterations; }

	/** iteraciones en que la reparacion exacta mejoro la solucion */
	int get_exact_improvements() const { return exact_improvements; }

	/**
	 * Construye el estado con los bloques indicados en sus posiciones
	 * (a partir del estado inicial s0)
	 */
	clpState* rebuild(const std::vector<PlacedBlock>& blocks) const;

private:

	/**
	 * Destruccion: separa los bloques de s en los que se mantienen (keep) y los
	 * k mas cercanos al centro de la region (removed)
	 */
	void destroy(const clpState& s, int k, std::vector<PlacedBlock>& keep, std::vector<PlacedBlock>& removed);

	/** region de la reparacion exacta */
	struct Region {
		AABB R;                      //caja envolvente de los bloques eliminados y del espacio vacio
		std::vector<AABB> obstacles; //bloques que se mantienen e intersectan R
		long free_volume;            //volumen de R sin los obstaculos
		long removed_volume;         //volumen de cajas de los bloques eliminados
		long potential;              //espacio vacio de R que podrian ocupar las cajas sin usar
		int boxes;                   //cajas de los bloques eliminados
	};

	Region region(const std::vector<PlacedBlock>& keep, const std::vector<PlacedBlock>& removed,
			bool has_void, const AABB& void_space) const;

	/** reparacion con BSG (nuevo estado o NULL) */
	clpState* bsg_repair(const std::vector<PlacedBlock>& keep, double tl, clock_t bt);

	/**
	 * Reparacion exacta: se llena con cajas la caja envolvente de los bloques eliminados
	 * y del espacio vacio usado como centro (los bloques que se mantienen y la
	 * intersectan son obstaculos) maximizando el
	 * volumen, partiendo de las cajas de los bloques eliminados. Si supera al volumen
	 * eliminado, se completa con greedy. Nuevo estado o NULL.
	 */
	clpState* exact_repair(const std::vector<PlacedBlock>& keep, const std::vector<PlacedBlock>& removed,
			double tl, clock_t bt);

	const clpState& s0;
	ActionEvaluator* evl;
	SearchStrategy& greedy;
	Shape shape;
	Center center;
	Repair repair;
	CpSolver* solver;
	ExactParams exact;
	int beams;
	int samples;
	double k0, kmax, growth;
	std::mt19937 rng;
	int iterations;
	int exact_improvements;

	//espacio vacio usado como centro de la ultima destruccion
	AABB void_space;
	bool has_void;

	//la ultima reparacion exacta no se hizo porque los bloques eliminados tienen
	//mas cajas que ExactParams::max_boxes
	bool too_large;

	//bloques de una caja: (tipo, l, w, h) -> bloque
	std::map< std::tuple<int,long,long,long>, const Block* > single_blocks;
};

} /* namespace clp */

#endif /* LNS_H_ */
