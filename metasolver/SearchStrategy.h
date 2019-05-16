/*
 * SearchStrategy.h
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <list>
#include <random>
#include <string>
#include "State.h"

#ifndef SEARCHSTRATEGY_H_
#define SEARCHSTRATEGY_H_

using namespace std;

namespace metasolver {

//srand(seed)

class Generator {
	std::default_random_engine generator;
	std::normal_distribution<double> distribution;
	double min;
	double max;
	double media;
	public:
	Generator(double mean, double stddev, double min, double max):
		distribution(mean, stddev), media(mean), min(min), max(max){}
		double operator ()(int r) {
			generator.seed(r);
	  	for(int i = 0; i <= 100; i++) {
	  		double number = this->distribution(generator);
	    	if (number >= this->min && number <= this->max) return number;
	    }
			return media;
	  }
	};


class tau_matrix{

private:
	//distribucion de feromona
	//la clave del mapa es el estado codificado
	//el valor es un vector de pares (media del valor del parámetro, cantidad de veces modificado)
	map<long, vector<pair <double, int> > > ph_distribution;
	vector<pair <double, double> > parameter_ranges;

	double fRand(double fMin, double fMax)
	{
	    double f = (double)rand() / RAND_MAX;
	    return fMin + f * (fMax - fMin);
	}

	double truncated_normal(double val, int n, pair<double,double> parameter_range){
			double min = parameter_range.first;
			double max = parameter_range.second;
			double stdDis=std_dev;
			if(std_dev ==0.0)
			   stdDis = (max-min)/(n);


			double media = val;
			//función que retorna el sampling de la distribucion normal
			Generator NormalGenerator(media, stdDis, min,max);
			return NormalGenerator(rand());
	}

public:
	tau_matrix(vector<pair <double, double> >& p) : parameter_ranges(p), iter_pheromone(0), std_dev(0.0){
	}

	//samplea parametros de acuerdo a distribucion normal asociada al estado
	vector <double> sample_param_vector(const State* s){
		//TODO: se obtiene el el estado codificado
		long state_code= s->get_code();
		vector <double> values(parameter_ranges.size());
		//cout << state_code << " ";
		if(ph_distribution.find(state_code)!=ph_distribution.end()){
				vector<pair <double, int> >& dist_params = ph_distribution[state_code];
				for(int i=0; i<dist_params.size();i++ ){
						//TODO: samplear de normal con media dist_params[i].first y desviación M/dist_params[i].second
						//truncar dentro del rango [parameter_ranges[i].first,parameter_ranges[i].second]
						values[i]= truncated_normal(dist_params[i].first, dist_params[i].second, parameter_ranges[i]);
						//cout << values[i] << "("<< dist_params[i].first <<  "," <<dist_params[i].second << ")";
						//values[i]=fRand(parameter_ranges[i].first,parameter_ranges[i].second);
				}
				//cout << endl;
		}else //random values in the range
			for(int i=0;i<values.size();i++)
				values[i]=fRand(parameter_ranges[i].first,parameter_ranges[i].second);

		return values;

	}

	//actualiza los parametros de distribucion (media y n) asociados al estado de
	// acuerdo a los valores de los parametros
	//Implementar aquí funcion para generar un graficos
	void add_pheromone(const long state_code, const vector<double>& parameter_values){
		iter_pheromone++;
		char *direccion;
		char dir[27] = "Values/StateN/ValuesM.txt";
		direccion = &dir[0];
		if(ph_distribution.find(state_code)!=ph_distribution.end()){
			vector<pair <double, int> >& dist_params = ph_distribution[state_code];
			for(int i=0; i<dist_params.size();i++ ){
				FILE *state;
				double media = dist_params[i].first;
				double n = dist_params[i].second;
				dist_params[i].first = //(n*media+parameter_values[i])/(n+1);
						alpha_media*media + (1.0-alpha_media)*parameter_values[i]; //  (n*media+parameter_values[i])/(n+1);
				dist_params[i].second++;
				cout << dist_params[i].first  << ",";
				//Largo de 27 caracteres para cada ruta
				if(write_report){
					*(direccion+12) = state_code+'0';
					*(direccion+20) = i+'0';
					//cout << dir << endl;
					state = fopen(dir,"a");
					fprintf(state,"%f %f %f \n",alpha_media,iter_pheromone, parameter_values[i]);
					fclose(state);
				}
			}
			cout << endl;
		}else{
			//you can only get here in the first iteration
			//erasing files for writting
			if(write_report){
				remove("Values/State0/Values0.txt");
				remove("Values/State0/Values1.txt");
				remove("Values/State0/Values2.txt");
				remove("Values/State0/Values3.txt");
				remove("Values/State0/Values4.txt");

				remove("Values/State1/Values0.txt");
				remove("Values/State1/Values1.txt");
				remove("Values/State1/Values2.txt");
				remove("Values/State1/Values3.txt");
				remove("Values/State1/Values4.txt");
				remove("Values/State2/Values0.txt");
				remove("Values/State2/Values1.txt");
				remove("Values/State2/Values2.txt");
				remove("Values/State2/Values3.txt");
				remove("Values/State2/Values4.txt");
			}
			vector<pair <double, int> > dist_params(parameter_values.size());
			for(int i=0; i<dist_params.size();i++ ){
				dist_params[i].first = parameter_values[i];
				dist_params[i].second = 1;
				ph_distribution[state_code]=dist_params;
			}
		}

	}
  static bool write_report;
  static double alpha_media;
  double iter_pheromone;
  double std_dev;
};

//TODO: refactorizar
class SearchStrategy {
public:
	SearchStrategy(ActionEvaluator* evl=NULL, double aco_alpha=0.0, double aco_beta=0.0, tau_matrix* tauM=NULL) :
		evl(evl), best_state(NULL), timelimit(0.0), begin_time(clock()), aco_alpha(aco_alpha), aco_beta(aco_beta), tauM(tauM) {} ;

	virtual ~SearchStrategy() {

	}


	double get_time(){
		return (double(clock()-begin_time)/double(CLOCKS_PER_SEC));
    }



	/*
	 * Initialize the variables of the specific strategy
	 */
	virtual void initialize (State* s=NULL) { best_state=s; }

	virtual void clean(list<State*>& S){ }

	/**
	 * Run the strategy
	 */
	virtual double run(State& s, double tl=99999.9, clock_t bt=clock()){
		list<State*> S;
		S.push_back(&s);
		initialize (&s);

		return run(S, tl, bt);
	}

	/**
	 * Run the strategy
	 */
	virtual double run(list<State*>& S, double tl=99999.9, clock_t bt=clock()){
		begin_time=bt;
		timelimit=tl;

		do{
			if(timelimit != 0.0 && get_time()>timelimit ){
				clean(S);
				return get_best_value() ;
			}
			list<State*> R=next(S);
			clean(S);
			S=R;
		}while(S.size()>0);

		return get_best_value() ;
	}

	/**
	 * Performs an iteration of the strategy
	 * @returns true if the search strategy has not finished yet
	 */
	virtual list<State*> next(list<State*>& S) { };

	virtual double get_best_value() const {
		if(best_state)
			return best_state->get_value();
		else return 0;
	}

	virtual  State* get_best_state() {
		return best_state;
	}

	/*
	 * Esta funcion "duplica" el esfuerzo de la estrategia.
	 * Si el esfuerzo es duplicado satisfactoriamente retorna true.
	 * (Implementar si la estrategia es compatible con el
	 * algoritmo de doble esfuerzo.)
	 */
	virtual bool double_effort() { return false; }

	int get_best_actions_ACO(const State& s, list< Action* >& bactions, int n);

	/**
	 * \brief return the n best actions according to the ActionEvaluator
	 */
	virtual int get_best_actions(const State& s, list< Action* >& bactions, int n);
	double timelimit;
protected:

	/*
	 * \brief return the best action according to the ActionEvaluator
	 */
	virtual Action* best_action(const State& s);

	//virtual Action* best_action_ACO(const State& s);




	State* best_state;
	clock_t begin_time;

	ActionEvaluator* evl;

	double aco_alpha;
	double aco_beta;



	tau_matrix* tauM;

};

} /* namespace clp */

#endif /* SEARCHSTRATEGY_H_ */
