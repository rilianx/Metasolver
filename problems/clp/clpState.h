/*
 * clpState.h
 *
 *  Created on: 4 jul. 2017
 *      Author: iaraya
 */

#ifndef CLPSTATE_H_
#define CLPSTATE_H_

#include <string>
#include <vector>
#include <list>

#include "State.h"
#include "BoxShape.h"
#include "Block.h"
#include "Container.h"

using namespace std;
using namespace metasolver;

namespace clp {

/**
 * Accion: ubicar un bloque en la esquina de anclaje de un espacio libre
 */
class clpAction : public Action{
public:
	clpAction(const Block& block, const Space& space) : block(block), space(space) { }

	virtual Action* clone() const{ return new clpAction(*this); }

	const Block& block;
	const Space space;
};

/**
 * Solucion parcial: el contenedor con los bloques ubicados, la cantidad de cajas
 * restantes de cada tipo y la lista de bloques que aun se pueden construir.
 */
class clpState : public State {
public:

	clpState(const clpState& S) : State(S), cont(S.cont), left(S.left),
		valid_blocks(S.valid_blocks), param(S.param), min_l(S.min_l), min_w(S.min_w), min_h(S.min_h), hash(S.hash) { }

	virtual State* clone() const{ return new clpState(*this); }

	virtual double get_value() const{
		return (double) cont.getOccupiedVolume() / (double) cont.getVolume();
	}

	virtual int get_best_actions(ActionEvaluator* evl, list< Action* >& bactions, int n) const;

	int get_n_valid_blocks() const {return valid_blocks.size();}

	/**
	 * Verifica la solucion: bloques dentro del contenedor, sin intersecciones y
	 * sin usar mas cajas que las disponibles
	 */
	bool is_feasible() const;

	/**
	 * Hash del conjunto de bloques ubicados (independiente del orden en que se ubicaron).
	 * Los estados con los mismos bloques en las mismas posiciones son equivalentes.
	 */
	virtual uint64_t get_hash() const { return (param==0)? hash : hash ^ (0x9ddfea08eb382d69ull*(uint64_t)param); }

	/** el estado hereda los parametros de la solucion final que sigue (ver BSG) */
	virtual void inherit(const State& final) { param = static_cast<const clpState&>(final).param; }

	//tipos de caja de la instancia
	static vector<const BoxShape*> types;

	//cantidad de cajas de cada tipo en la instancia
	static vector<int> initial_boxes;

	//los espacios en los que no cabe ningun bloque se eliminan al buscar acciones
	mutable Container cont;

	//cantidad restante de cajas de cada tipo
	vector<int> left;

	vector<const Block*> valid_blocks;

	//indice de los parametros de la funcion de evaluacion (VCS_Function::add_params)
	int param;

	friend clpState* new_state(string file, int instance, double min_fr, int max_bl);

protected:

	virtual void _transition(const Action& action);

private:

	clpState(long L, long W, long H) : cont(L, W, H), param(0), min_l(L), min_w(W), min_h(H), hash(0x9e3779b97f4a7c15ull) { }

	/**
	 * Elimina de valid_blocks los bloques que no se pueden construir con las cajas restantes
	 */
	void update_valid_blocks();

	void update_min_dim();

	//menor dimension de las cajas restantes (los espacios menores se descartan)
	long min_l, min_w, min_h;

	uint64_t hash;
};

/**
 * Lee la instancia (formato BR) y genera los bloques
 */
clpState* new_state(string file, int instance, double min_fr=0.98, int max_bl=10000);

} /* namespace clp */

#endif /* CLPSTATE_H_ */
