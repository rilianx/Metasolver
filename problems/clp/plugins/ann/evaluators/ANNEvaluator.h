/*
 * ANNevaluator.h
 *
 *  Created on: 14 mar. 2018
 *      Author: iaraya
 */

#include <iostream>
#include "ActionEvaluator.h"
#include "Block.h"
#include "clpState.h"
#include "VCS_Function.h"

#ifndef EVALUATORS_ANNEVALUATOR_H_
#define EVALUATORS_ANNEVALUATOR_H_

extern "C" {
	#include "../glue/glue.h"
}

using namespace metasolver;

namespace clp {

	class ANNEvaluator : public ActionEvaluator {
	public:

		ANNEvaluator(int L, int W, VCS_Function* vcs) : ActionEvaluator(0.0), L(L), W(W) {

			matrix = new int*[L];
			for(int i=0; i<L; i++)
				matrix[i] = new int[W];
			
			vcs_f = vcs;

			// Inicializando red
			// /home/braulio/Metasolver/problems/clp/plugins/ann/annCom.py
			const char* pathScript = "/home/braulio/MetasolverAnn/problems/clp/plugins/ann/";
			const char* nameScript = "annCom";

			pyHandler.pModule = glue_initPyHandler(pyHandler, pathScript, nameScript);
			if(pyHandler.pModule == NULL)
				cout << "\033[1;31mERROR: pModule es nulo en el constructor\033[0m\n";
		}

		virtual ~ANNEvaluator(){
			//delete[][] matrix;
			//for(int i=0; i<L; i++)
			//delete[] matrix[i];
		}

		virtual void initialize(const State& s){
			const char *nFuncInput1 = "setValueInput1"; // Nombre de la funcion para actualizar input 1 de la red (En Python)
			const char *nFuncInput2 = "setValueInput2"; // Nombre de la funcion para actualizar input 2 de la red (En Python)

			const clpState* state = dynamic_cast<const clpState*>(&s);

			for(int i=0; i<L; i++)
				for(int j=0; j<W; j++)
					matrix[i][j]= 0;

			if(pyHandler.pModule == NULL)
				cout << "\033[1;31mERROR: pModule es nulo en la inicializacion\033[0m\n";

			// Generando input 1
			if(state->cont->blocks->size()){
			    AABB block = state->cont->blocks->top();
				while(true){
					for(long i = block.getXmin(); i < block.getXmax(); i++) {
						for (long j = block.getYmin(); j < block.getYmax(); j++) {
							glue_putInput(pyHandler, nFuncInput1, i, j, block.getZmax());
						}
					}
					if(state->cont->blocks->has_next()){
						block = state->cont->blocks->next();
					} else break;
				}
			}

			list< Action* > actions;
			s.get_actions(actions);

            // Generando el input 2
			glue_resetInput2(pyHandler);
			const Space& sp = state->cont->spaces->top();
			for(long i = sp.getXmin(); i < sp.getXmax(); i++){
				for(long j = sp.getYmin(); j < sp.getYmax(); j++){
					glue_putInput(pyHandler, nFuncInput2, i, j, 1);
				}
			}

			// Obteniendo prediccion de la red
			glue_operate(pyHandler);

			// TODO: en esta parte debe setear los parametros del otro evaluator con la salida de la red
			// VCS_Function en problems/clp/evaluators

			vcs_f->set_alpha(glue_getAlpha(pyHandler));
			vcs_f->set_gamma(glue_getGamma(pyHandler));
			vcs_f->set_p(glue_getP(pyHandler));
			vcs_f->set_beta(glue_getBeta(pyHandler));

		}

		virtual double eval_action(const State& s, const Action& a){
			return vcs_f->eval_action(s, a);
		}


	private:
		double prob[5][5][5];
		int** matrix;
		int L,W;

		PyHandler pyHandler;
		VCS_Function* vcs_f;
	};

} /* namespace clp */

#endif /* EVALUATORS_ANNEVALUATOR_H_ */
