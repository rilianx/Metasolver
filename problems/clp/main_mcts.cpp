/*
 * main_clp.cpp
 *
 *  Created on: 29 jun. 2017
 *      Author: iaraya
 */

#include <iostream>
#include "clpState.h"
#include "clpStatekd.h"
#include "BlockSet.h"
#include "BSG_midBSG.h"
#include "VCS_Function.h"
#include "VCS_Function.h"
#include "SpaceSet.h"
#include "Greedy.h"
#include "DoubleEffort.h"
#include "GlobalVariables.h"
#include "MCTS.h"

bool global::TRACE = false;

using namespace std;

int main(int argc, char** argv){

	string file=argv[1];
	int inst=atoi(argv[2]);
	double min_fr=atof(argv[3]); //<-- 0.98 o 1.0

	int max_time=atoi(argv[4]); //500
    double alpha=atof(argv[5]); //4.0
    double beta=atof(argv[6]); //1.0
    double gamma=atof(argv[7]); //0.2
    double p=atof(argv[8]); //0.04
    double delta=atof(argv[9]); //1.0
	double f=atof(argv[10]); //0.0
    double r=atof(argv[11]); //0.0
    bool fsb=(atoi(argv[12])==1); //0
    bool kdtree=atoi(argv[13]); //0



    cout << "cargando la instancia..." << endl;

    clpState* s0 = new_state(file,inst, min_fr, 10000);

    cout << "cargo la instancia"<<endl;
    clock_t begin_time=clock();

    cout <<"Funcion evaluadora"<<endl;
    VCS_Function* vcs = new VCS_Function(s0->nb_left_boxes, *s0->cont,
    alpha, beta, gamma, p, delta, f, r);

    cout <<"Funcion greedy"<<endl;
    SearchStrategy *gr = new Greedy (vcs);

    cout <<"Creando montecarlo"<<endl;
    MCTS *mtcs=new  MCTS(vcs,gr);

    cout <<"Corriendo montecarlo"<<endl;
    double value= mtcs->run(*s0,(double)max_time,begin_time);

    cout <<"best value founded:"<<value<<endl;
    cout <<"algoritmo MTCS v2"<<endl;


	return 0;
}
