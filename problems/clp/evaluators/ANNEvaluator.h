/*
 * ANNevaluator.h
 *
 *  Created on: 14 mar. 2018
 *      Author: iaraya
 */

#include "ActionEvaluator.h"

#ifndef EVALUATORS_ANNEVALUATOR_H_
#define EVALUATORS_ANNEVALUATOR_H_

using namespace metasolver;

namespace clp {

class ANNEvaluator : public ActionEvaluator {
public:

	ANNEvaluator() {
		//TODO: Inicializar la red
	}

	virtual ~ANNEvaluator(){

	}

	virtual void initialize(const State& s){
		clpState* state = dynamic_cast<const clpAction*>(&s);
		AABB block = state->cont->blocks->top();
		block.getXmin(); block.getXmax();
		block.getYmin(); block.getYmax();
		block.getZmax();
		state->cont->blocks->next();
		//TODO: Guardar estado en matriz para la red

		//TODO: Pedirle a la red que haga su predicción y la guarde en un archivo (vector)
		//TODO: Leer el vector y guardarlo en prob[5][5][5]


	}

	virtual double eval_action(const State& s, const Action& a){
		const Block& b = dynamic_cast<const clpAction*>(&a)->block;
		const Space& sp =dynamic_cast<const clpAction*>(&a)->space;

		if(!(b <= sp)) return -1.0;

		//clpState* state = dynamic_cast<const clpAction*>(&s);
		//TODO: recibir vector y mapearlo a dimensiones del espacio

		//evaluar bloque de acuerdo a cajita de probabilidades
		sp.getH(); sp.getW(); sp.getL();
		b.getH(); b.getW(); b.getL();

	}


private:
	double prob[5][5][5];



};

} /* namespace clp */

#endif /* EVALUATORS_ANNEVALUATOR_H_ */
