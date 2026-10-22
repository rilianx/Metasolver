/*
 * VCS_Function.h
 *
 *  Created on: 2 jun. 2017
 *      Author: iaraya
 */

#ifndef VCS_FUNCTION_H_
#define VCS_FUNCTION_H_

#include <vector>
#include <cmath>
#include "ActionEvaluator.h"
#include "BoxShape.h"
#include "Block.h"
#include "Space.h"

namespace clp {

class clpState;

/**
 * Funcion de evaluacion VCS (Araya et al. 2017):
 *   vol^delta * (1-loss)^beta * cs^alpha * (1/n_boxes)^gamma
 * donde loss es la perdida de volumen estimada y cs la superficie de
 * pseudo-contacto relativa del bloque ubicado en el espacio.
 */
class VCS_Function : public metasolver::ActionEvaluator {
public:
	/**
	 * @param blocks bloques de la instancia (indexados por Block::id)
	 * @param fix_knapsack si es true, la mochila de la perdida de volumen considera los
	 * largos de todas las orientaciones permitidas de cada caja (la version original
	 * solo considera las orientaciones LWH, WHL y HLW, lo que omite largos validos en
	 * los ejes x e y cuando la caja no puede ponerse con su largo vertical)
	 */
	VCS_Function(const std::vector<const BoxShape*>& types, const std::vector<int>& nb_boxes,
			const std::vector<const Block*>& blocks,
			long L, long W, long H, double alpha=4.0, double beta=1.0,
			double gamma=0.2, double p=0.04, double delta=1.0, bool fix_knapsack=false);

	/**
	 * Prepara la evaluacion de bloques en el espacio sp: se buscan los bloques
	 * ubicados que pueden estar en pseudo-contacto con algun bloque dentro de sp
	 * (Algoritmo 4.2 de la disertacion de L. Guesser)
	 */
	void set_space(const clpState& s, const Space& sp);

	/**
	 * Evalua el bloque b ubicado en la posicion (x,y,z) del espacio indicado en set_space.
	 * Si se sabe que la evaluacion no supera threshold (cota sin la perdida de
	 * volumen), retorna -1 sin calcular la perdida
	 */
	double eval(const clpState& s, const Block& b, const Space& sp, long x, long y, long z,
			double threshold=-HUGE_VAL);

private:

	double CS_p(const clpState& s, const Block& b, long x, long y, long z);

	long surface_in_contact(const AABB& b, const AABB& bi) const;
	long surface_in_contact_container(const AABB& b, const AABB& bounds) const;

	/* Perdida de volumen */

	double Loss(const std::vector<int>& left, const Block& block, const Space& free_space) const;

	/**
	 * Retorna la perdida en la dimension d de un espacio residual de largo resX
	 */
	long compute_lossX(int d, const std::vector<int>& left, const Block& block, long resX) const;

	/**
	 * Solucion de la mochila en una dimension:
	 * mX[i]: maximo largo <= i que se puede llenar con cajas
	 * tipos de caja que permiten llenar exactamente el largo i:
	 *   list[offset[i]] ... list[offset[i+1]-1]  (arreglo contiguo)
	 */
	struct LossData {
		std::vector<long> mX;
		std::vector<int> offset;
		std::vector<int> list;
	};

	/**
	 * Resuelve el problema de la mochila en la dimension dim (0:L, 1:W, 2:H) con capacidad X.
	 */
	void compute_mX(const std::vector<const BoxShape*>& types, const std::vector<int>& nb_boxes,
			long X, std::vector<long>& mX, std::vector< std::vector<int> >& listX, int dim);

	bool fix_knapsack;

	LossData ld[3];

	//pow(vol,delta) y pow(n,gamma) de cada bloque (constantes durante la busqueda)
	std::vector<double> pow_vol, pow_n;

	//cota de (1-loss)^beta en el espacio actual
	double loss_bound;

	//bloques ubicados en la vecindad del espacio actual
	std::vector<const PlacedBlock*> neighbors;

	//contenedor del espacio actual
	AABB bounds;

	double alpha, beta, gamma, p, delta;
};

} /* namespace clp */

#endif /* VCS_FUNCTION_H_ */
