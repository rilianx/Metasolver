/*
 * VCS_Function.h
 *
 *  Created on: 2 jun. 2017
 *      Author: iaraya
 */

#ifndef VCS_FUNCTION_H_
#define VCS_FUNCTION_H_

#include <vector>
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
	VCS_Function(const std::vector<const BoxShape*>& types, const std::vector<int>& nb_boxes,
			long L, long W, long H, double alpha=4.0, double beta=1.0,
			double gamma=0.2, double p=0.04, double delta=1.0);

	/**
	 * Prepara la evaluacion de bloques en el espacio sp: se buscan los bloques
	 * ubicados que pueden estar en pseudo-contacto con algun bloque dentro de sp
	 * (Algoritmo 4.2 de la disertacion de L. Guesser)
	 */
	void set_space(const clpState& s, const Space& sp);

	/** evalua el bloque b ubicado en el espacio indicado en set_space */
	double eval(const clpState& s, const Block& b, const Space& sp);

private:

	double CS_p(const clpState& s, const Block& b, const Space& sp);

	long surface_in_contact(const AABB& b, const AABB& bi) const;
	long surface_in_contact_container(const AABB& b, long L, long W, long H) const;

	/* Perdida de volumen */

	double Loss(const std::vector<int>& left, const Block& block, const Space& free_space) const;

	/**
	 * Retorna la perdida en la dimension (lossX) de un espacio residual de largo resX
	 */
	long compute_lossX(const std::vector<int>& left, const Block& block, long resX,
			const std::vector<long>& mX, const std::vector< std::vector<int> >& listX) const;

	/**
	 * Resuelve el problema de la mochila en la dimension dim (0:L, 1:W, 2:H) con capacidad X.
	 * mX[i]: maximo largo <= i que se puede llenar con cajas
	 * listX[i]: tipos de caja que permiten llenar exactamente el largo i
	 */
	void compute_mX(const std::vector<const BoxShape*>& types, const std::vector<int>& nb_boxes,
			long X, std::vector<long>& mX, std::vector< std::vector<int> >& listX, int dim);

	std::vector<long> mL, mW, mH;
	std::vector< std::vector<int> > listL, listW, listH;

	//bloques ubicados en la vecindad del espacio actual
	std::vector<const PlacedBlock*> neighbors;

	double alpha, beta, gamma, p, delta;
};

} /* namespace clp */

#endif /* VCS_FUNCTION_H_ */
