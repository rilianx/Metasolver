/*
 * ANNevaluator.h
 *
 *  Created on: 14 mar. 2018
 *      Author: iaraya
 */

#include "ActionEvaluator.h"
#include "Block.h"
#include "clpState.h"
#include "../glue/glue.h"

#ifndef EVALUATORS_ANNEVALUATOR_H_
#define EVALUATORS_ANNEVALUATOR_H_

using namespace metasolver;

namespace clp {

	class ANNEvaluator : public ActionEvaluator {
	public:

		ANNEvaluator(int L, int W) : ActionEvaluator(0.0), L(L), W(W) {
			matrix = new int*[L];
			for(int i=0; i<L; i++)
				matrix[i] = new int[W];


			// Inicializando red
			const char* pathScript = "/home/braulio/CLionProjects/Metasolver/problems/clp/plugins/ann/";
			const char* nameScript = "AnnScriptRun.py";

			pyHandler = glue_createPyHandler();
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
			//const char *nFuncInput2 = "putInput2"; // Nombre de la funcion para actualizar input 2 de la red (En Python)

			const clpState* state = dynamic_cast<const clpState*>(&s);
			AABB block = state->cont->blocks->top();
			block.getXmin(); block.getXmax();
			block.getYmin(); block.getYmax();
			block.getZmax();

			for(int i=0; i<L; i++)
				for(int j=0; j<W; j++)
					matrix[i][j]= 0;

			do{
				for(long i = block.getXmin(); i < block.getXmax(); i++) {
					for (long j = block.getYmin(); j < block.getYmax(); j++) {
						glue_putInput(pyHandler, nFuncInput1, i, j, block.getZmax()); // matrix[i][j] = block.getZmax();
					}
				}
				if(state->cont->blocks->has_next())
					block=state->cont->blocks->next();

			}while(state->cont->blocks->has_next());

			//TODO: buscar forma de generar el input 2

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

			//if(!(b <= sp)) return -1.0;

			//clpState* state = dynamic_cast<const clpAction*>(&s);
			//TODO: recibir vector y mapearlo a dimensiones del espacio

			//evaluar bloque de acuerdo a cajita de probabilidades
			//sp.getH(); sp.getW(); sp.getL();
			//b.getH(); b.getW(); b.getL();

			return double_rand();

		}


	private:
		double prob[5][5][5];
		int** matrix;
		int L,W;

		PyHandler pyHandler;
	};

} /* namespace clp */

#endif /* EVALUATORS_ANNEVALUATOR_H_ */
