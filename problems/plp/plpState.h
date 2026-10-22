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
#include "Placer.h"
#include "Solution.h"

using namespace metasolver;

namespace plp {

class plpAction : public Action {
public:
	plpAction(int type, const Vec3& t) : type(type), t(t) { }

	virtual Action* clone() const { return new plpAction(*this); }

	int type;
	Vec3 t;
};

/**
 * Evaluacion de una accion (mayor es mejor):
 *   alpha*log(V/Vmax) - beta*z/H0 - gamma*max(0, ztop - top)/H0
 * V: volumen de la pieza, Vmax: volumen de la pieza mas grande, z: base de la pieza,
 * ztop: tope de la pieza, top: altura actual de la carga, H0: cota inferior de la altura.
 */
class PLP_Function : public ActionEvaluator {
public:
	PLP_Function(Placer& placer, double alpha=1.0, double beta=1.0, double gamma=1.0);

	double eval(int type, const Vec3& t, double top) const;

	Placer& placer;

private:
	double alpha, beta, gamma;
	double vmax, h0;
};

class plpState : public State {
public:

	/** estado inicial (contenedor vacio) */
	plpState(const Instance& inst);

	virtual State* clone() const { return new plpState(*this); }

	/** densidad (volumen de las piezas / volumen hasta la altura de la carga) si se ubicaron todas las piezas; 0 si no */
	virtual double get_value() const;

	virtual int get_best_actions(ActionEvaluator* evl, std::list< Action* >& bactions, int n) const;

	virtual uint64_t get_hash() const { return hash; }

	Solution get_solution() const;

	double get_height() const { return top; }

	int get_n_left() const { return n_left; }

protected:

	virtual void _transition(const Action& action);

private:

	const Instance* inst;

	struct Item { int type; Vec3 t; };
	std::vector<Item> items;

	std::vector<PlacedPart> placed;

	//piezas restantes de cada tipo
	std::vector<int> left;
	int n_left;

	double top;
	double volume;

	uint64_t hash;
};

} /* namespace plp */

#endif /* PLP_PLPSTATE_H_ */
