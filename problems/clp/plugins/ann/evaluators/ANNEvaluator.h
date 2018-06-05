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
			// /home/braulio/Metasolver/problems/clp/plugins/ann/annCom.py
			const char* pathScript = "/home/braulio/Metasolver/problems/clp/plugins/ann/";
			const char* nameScript = "annCom";

			pyHandler.pModule = glue_initPyHandler(pyHandler, pathScript, nameScript);
			if(pyHandler.pModule == NULL)
				cout << "\033[1;31mERROR: pModule es nulo en el constructor\033[0m\n";
			//TODO: Inicializar la red
		}

		virtual ~ANNEvaluator(){
			//delete[][] matrix;
			//for(int i=0; i<L; i++)
			//delete[] matrix[i];
		}

		virtual void initialize(const State& s){
			//cout << "initialize init" << endl;
			const char *nFuncInput1 = "setValueInput1"; // Nombre de la funcion para actualizar input 1 de la red (En Python)
			const char *nFuncInput2 = "setValueInput2"; // Nombre de la funcion para actualizar input 2 de la red (En Python)

			const clpState* state = dynamic_cast<const clpState*>(&s);
			AABB block = state->cont->blocks->top();
			block.getXmin(); block.getXmax();
			block.getYmin(); block.getYmax();
			block.getZmax();

			//cout << "Init matrix" << endl;
			for(int i=0; i<L; i++)
				for(int j=0; j<W; j++)
					matrix[i][j]= 0;
			//cout << "end" << endl;

			// FIXME: borrar esto
			if(pyHandler.pModule == NULL)
				cout << "\033[1;31mERROR: pModule es nulo en la inicializacion\033[0m\n";

            // Generando el input 1
			//cout << "Init input 1" << endl;
			while(state->cont->blocks->has_next()){
				//cout << block.getXmin() << endl;
				//cout << block.getZmax() << endl;
				//cout << block.getXmax() << endl;
				//cout << block.getYmax() << endl;
				for(long i = block.getXmin(); i < block.getXmax(); i++) {
					for (long j = block.getYmin(); j < block.getYmax(); j++) {
						//cout << "Input 1" << endl;
						//cout << block.getZmax() << endl;
						glue_putInput(pyHandler, nFuncInput1, i, j, block.getZmax()); // matrix[i][j] = block.getZmax();
					}
				}
				if(state->cont->blocks->has_next())
					block=state->cont->blocks->next();
			}
			//cout << "end" << endl;

			list< Action* > actions;
			s.get_actions(actions);

            // Generando el input 2
			//cout << "Init input 2" << endl;
			glue_resetInput2(pyHandler);
			const Space& sp = state->cont->spaces->top();
			for(long i = sp.getXmin(); i < sp.getXmax(); i++){
				for(long j = sp.getYmin(); j < sp.getYmax(); j++){
					glue_putInput(pyHandler, nFuncInput2, i, j, 1);
				}
			}
			//cout << "end" << endl;
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

			
			//cout << "operaint" << endl;
			glue_operate(pyHandler);
			//cout << "end" << endl;

			// Actualizando cubo de soluciones
			const int xMax = 5;
			const int yMax = 5;
			const int zMax = 5;
			for(long i = 0; i < xMax; i++)
				for(long j = 0; j < yMax; j++)
					for(long k = 0; k < zMax; k++)
						prob[i][j][k] = glue_getSolution(pyHandler, i, j, k);
			//cout << "initialize end" << endl;
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
			return eval; //(rand()%10000)/10000.0;//eval;
		}


	private:
		double prob[5][5][5];
		int** matrix;
		int L,W;

		PyHandler pyHandler;
	};

} /* namespace clp */

#endif /* EVALUATORS_ANNEVALUATOR_H_ */
