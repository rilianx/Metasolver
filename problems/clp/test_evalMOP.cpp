/*
 * main_clp.cpp
 *
 *  Created on: 29 jun. 2017
 *      Author: iaraya
 */

#include <iostream>
//#include "objects/State.cpp"
#include "clpState.h"
//#include "plugins/mop/MO_clpBasicEvaluator.h"
#include "VCS_Function.h"
#include "SpaceSet.h"
#include "Greedy.h"
#include "DoubleEffort.h"
#include "GlobalVariables.h"
#include "BSGMOP.h"

bool global::TRACE = false;

using namespace std;

//State* new_state(string file, int instance);

int main(int argc, char** argv){

	string file=argv[1];
	double min_fr=atof(argv[2]);
	int max_time=atoi(argv[3]);

    double alpha=atof(argv[4]); //0.0 - 10.0
    double beta=atof(argv[5]); //0.0 - 10.0
    double gamma=atof(argv[6]); //0.0 - 1.0
    double p=atof(argv[7]); //0.0 - 0.1
    double delta=atof(argv[8]); //0.0 - 10.0
    double r=atof(argv[9]);

    cout << endl <<  "******* Parameters *********" << endl;
    cout << "file:" << file <<  endl;
    cout << "min_fr:" << min_fr << endl;
    cout << "max_time:" << max_time << endl;
    cout << "alpha:" << alpha << endl;
    cout << "beta:" << beta << endl;
    cout << "gamma:" << gamma << endl;
    cout << "p:" << p << endl;
    cout << "delta:" << delta << endl;
    cout << "r:" << r << endl;
    cout << "*****************************" << endl << endl;

	srand(1);

	//SpaceSet::random_spaces=true;

  map<double, pair<double, double>> sum;

  for(int inst=0; inst<100; inst++){
    //cout << inst << endl;
    //a las cajas se les inicializan sus pesos en 1
    clpState* s0 = new_state(file, inst, min_fr, 10000, clpState::BR);

    VCS_Function* vcs = new VCS_Function(s0->nb_left_boxes, *s0->cont,
    alpha, beta, gamma, p, delta);

    SearchStrategy *gr = new Greedy (vcs,0,0);

    for(double theta=0.0; theta<=4.0 ; theta+=0.1){
      State& s_copy= *s0->clone();
      vcs->set_theta(theta);
      gr->run(s_copy, max_time) ;

      const State* s=gr->get_best_state();
      sum[theta].first+= s->get_value();
      sum[theta].second += s->get_value2();
      //cout << s->get_value2() << endl;
      delete &s_copy;
    }

    delete gr;
    delete vcs;
    delete s0;
  }

  for(double f=0.0; f<=4.0 ; f+=0.1){
    cout <<  sum[f].second/100.0  << endl;
  }



	//dynamic_cast<const clpState*>(de->get_best_state())->cont.MatLab_print();


}
