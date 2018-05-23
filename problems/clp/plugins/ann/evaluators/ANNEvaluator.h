/*
 * ANNevaluator.h
 *
 *  Created on: 14 mar. 2018
 *      Author: iaraya
 */

#include "ActionEvaluator.h"
#include "Block.h"
#include "clpState.h"
//#include "glue.h"


//#include "glue.c"

#ifndef EVALUATORS_ANNEVALUATOR_H_
#define EVALUATORS_ANNEVALUATOR_H_

extern "C" {
	#include "../glue/glue.h"
}

using namespace metasolver;

namespace clp {

	class ANNEvaluator : public ActionEvaluator {
	public:

		ANNEvaluator(int L, int W) : ActionEvaluator(0.0), L(L), W(W) {

			matrix = new int*[L];
			for(int i=0; i<L; i++)
				matrix[i] = new int[W];


			// Inicializando red
			const char* pathScript = "/home/iaraya/clp/clp2017/problems/clp/plugins/ann/";
			const char* nameScript = "annCom";

			PyHandler pyHandler;
			glue_initPyHandler(pyHandler, pathScript, nameScript);
			//TODO: Inicializar la red
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
			AABB block = state->cont->blocks->top();
			block.getXmin(); block.getXmax();
			block.getYmin(); block.getYmax();
			block.getZmax();

			for(int i=0; i<L; i++)
				for(int j=0; j<W; j++)
					matrix[i][j]= 0;

            // Generando el input 1
			do{
				for(long i = block.getXmin(); i < block.getXmax(); i++) {
					for (long j = block.getYmin(); j < block.getYmax(); j++) {
						glue_putInput(pyHandler, nFuncInput1, i, j, block.getZmax()); // matrix[i][j] = block.getZmax();
					}
				}
				if(state->cont->blocks->has_next())
					block=state->cont->blocks->next();

			}while(state->cont->blocks->has_next());

			list< Action* > actions;
			s.get_actions(actions);

            // Generando el input 2
            // FIXME: los valores del input 2 fuera del rango de este, no se actualizan
			const Space& sp = state->cont->spaces->top();
			for(long i = sp.getXmin(); i < sp.getXmax(); i++){
				for(long j = sp.getYmin(); j < sp.getYmax(); j++){
					glue_putInput(pyHandler, nFuncInput2, i, j, 1);
				}
			}
			//sp.getXmin()
			//sp.getXmax()
			//sp.getYmin()
			//sp.getYmax()

			/*for(int i=0; i<L; i++){
                for(int j=0; j<W; j++)
                    printf("%3d",matrix[i][j]);
                cout << endl;
            }
            cout << endl;*/

			glue_operate(pyHandler);

			// Actualizando cubo de soluciones
			const int xMax = 5;
			const int yMax = 5;
			const int zMax = 5;
			for(long i = 0; i < xMax; i++)
				for(long j = 0; j < yMax; j++)
					for(long k = 0; k < zMax; k++)
						prob[i][j][k] = glue_getSolution(pyHandler, i, j, k);

		}

		virtual double eval_action(const State& s, const Action& a){
			const Block& b = dynamic_cast<const clpAction*>(&a)->block;
			const Space& sp =dynamic_cast<const clpAction*>(&a)->space;
            double eval = 0;
            double l = b.getL() * (5.0/sp.getL());
            double w = b.getW() * (5.0/sp.getW());
            double h = b.getH() * (5.0/sp.getH());

            // Actualizando cubo de soluciones
            const int xMax = 5;
            const int yMax = 5;
            const int zMax = 5;
            for(long i = 0; i < xMax; i++) {
                for (long j = 0; j < yMax; j++) {
                    for (long k = 0; k < zMax; k++) {
                        eval += prob[i][j][k] *
                                (1 - sqrt(pow(l - i, 2) + pow(w - j, 2) + pow(h - k, 2)) /
                                sqrt(pow(xMax, 2) + pow(yMax, 2) + pow(zMax, 2)));
                    }
                }
            }

			return eval;

		}


	private:
		double prob[5][5][5];
		int** matrix;
		int L,W;

		PyHandler pyHandler;
	};

} /* namespace clp */

#endif /* EVALUATORS_ANNEVALUATOR_H_ */
