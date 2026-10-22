/*
 * plpState.h
 *
 * Estado del BSG para la carga de poliedros (3D irregular strip packing, sin rotaciones):
 * las piezas ubicadas y las que faltan por ubicar. Una accion ubica una pieza en una
 * posicion factible obtenida por el Placer.
 */

#ifndef PLP_PLPSTATE_H_
#define PLP_PLPSTATE_H_

#include <vector>
#include "State.h"
#include "Greedy.h"
#include "Placer.h"
#include "Solution.h"

using namespace metasolver;

namespace plp {

/**
 * Ubica una pieza del tipo type en t y, si type2 >= 0, una segunda pieza del tipo type2 en t2
 * (pseudobloque).
 */
class plpAction : public Action {
public:
	plpAction(int type, const Vec3& t, int type2=-1, const Vec3& t2=Vec3()) : type(type), t(t), type2(type2), t2(t2) { }

	virtual Action* clone() const { return new plpAction(*this); }

	int type;
	Vec3 t;
	int type2;
	Vec3 t2;
};

/**
 * Evaluacion de una accion (mayor es mejor):
 *   alpha*log(V/Vmax) - beta*z/H0 - gamma*max(0, ztop - top)/H0
 * V: volumen de la forma (pieza o pseudobloque), Vmax: volumen de la pieza mas grande, z: base,
 * ztop: tope de la pieza, top: altura actual de la carga, H0: cota inferior de la altura.
 */
class PLP_Function : public ActionEvaluator {
public:
	PLP_Function(Placer& placer, double alpha=1.0, double beta=1.0, double gamma=1.0);

	/** evaluacion de la forma p del Placer (pieza o pseudobloque) en t */
	double eval(int p, const Vec3& t, double top) const;

	Placer& placer;

private:
	double alpha, beta, gamma;
	double vmax, h0;
};

/**
 * Greedy que compacta cada solucion completa con un LP global (Placer::compact).
 * El BSG compara entonces las soluciones compactadas.
 */
class CompactGreedy : public Greedy {
public:
	CompactGreedy(PLP_Function* evl, class LPBackend* lp, int iterations=5) :
		Greedy(evl), lp(lp), iterations(iterations) { }

	virtual double run(State& s, double tl=99999.9, clock_t bt=clock());

	/** estrategias que se detienen cuando el greedy ubica todas las piezas en un contenedor de altura fija */
	std::vector<SearchStrategy*> stop_when_complete;

private:
	class LPBackend* lp;
	int iterations;
};

class plpState : public State {
public:

	/**
	 * Estado inicial (contenedor vacio). Con hmax finito el contenedor tiene altura hmax
	 * (variante knapsack: maximizar el volumen cargado); si no, la altura es abierta.
	 */
	plpState(const Instance& inst, double hmax=std::numeric_limits<double>::infinity());

	virtual State* clone() const { return new plpState(*this); }

	/**
	 * Altura abierta: densidad (volumen / (W*D*altura de la carga)) si se ubicaron todas las piezas; 0 si no.
	 * Altura hmax: volumen cargado / volumen total (menos 1e-3*altura/hmax, para desempatar).
	 */
	virtual double get_value() const;

	bool is_knapsack() const { return hmax < std::numeric_limits<double>::infinity(); }

	/** quita la altura del contenedor (p.ej., para completar una carga parcial sobre la carga) */
	void open_height() { hmax = std::numeric_limits<double>::infinity(); }

	virtual int get_best_actions(ActionEvaluator* evl, std::list< Action* >& bactions, int n) const;

	virtual uint64_t get_hash() const { return hash; }

	Solution get_solution() const;

	/** piezas (indices de Instance::pieces) y posiciones de la solucion actual */
	void get_positions(std::vector<int>& pieces, std::vector<Vec3>& t) const;

	/**
	 * Reemplaza las posiciones de una solucion completa (p.ej., despues de compactarla).
	 * El camino de acciones no cambia; el valor y la solucion usan las nuevas posiciones.
	 */
	void set_positions(const std::vector<Vec3>& t);

	double get_height() const { return top; }

	int get_n_left() const { return n_left; }

protected:

	virtual void _transition(const Action& action);

private:

	void add_item(int type, const Vec3& t);

	const Instance* inst;

	struct Item { int type; Vec3 t; };
	std::vector<Item> items;

	std::vector<PlacedPart> placed;

	//piezas restantes de cada tipo
	std::vector<int> left;
	int n_left;

	double top;
	double volume;

	//altura del contenedor (infinito: altura abierta)
	double hmax;

	uint64_t hash;
};

} /* namespace plp */

#endif /* PLP_PLPSTATE_H_ */
