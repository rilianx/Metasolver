/*
 * MCTS.h
 *
 *  Created on: 03-01-2018
 *      Author: investigador
 */

#ifndef MCTS_H_
#define MCTS_H_

#include <time.h>
#include "MctsNode.h"
#include "SearchStrategy.h"
#include "chi_squared.hpp"
#include "normal.hpp"
namespace metasolver {

class State;
class Stimator{
public:
	Stimator():slope(0),intercept(0),alpha(0.05){};
	double stimate_standart_deviation(double volume);
	void addPoint(vector<double>,double);
	double normal_cdf(double);
	double z_value(double,double,double);
	double probability_greater_than(double,double,double);
	void addPoint(double,double);

private:
	double slope,intercept,alpha;//coeficientes de la recta, a: pendiente, b:
	vector <pair<double,double>> points;
	void update_coeficients();
	double standart_deviation(vector<double>);
	double meean(vector<double>);

};
/*
class mcts_node_{
public:
	double getValue(){
		return value;
		}
	void setValue(double val){
		value=val;
	}
private:
	double value;
	MctsNode *node;

};

class Compare_node{
public:
    bool operator() (mcts_node_ a, mcts_node_ b)
    {
        return 	a.getValue()>=b.getValue();
    }
};
*/
class MCTS {
public:
	double bestValue;
	Stimator stimator;

	//priority_queue<mcts_node_, vector<mcts_node_>, Compare_node> node_queue;

	MCTS();
	MCTS(ActionEvaluator* evl,SearchStrategy* greedy) :
	evl(evl), greedy(greedy), best_node(NULL), timelimit(0.0), begin_time(clock()),root(NULL),best_state(NULL),bestValue(-1)
	{} ;
	MctsNode* bestChild(MctsNode*);
	double run(State&,double,clock_t);
	double getTime(){
		return (double(clock()-begin_time)/double(CLOCKS_PER_SEC));
	}
	MctsNode* selection(MctsNode*);
	MctsNode* selectRandom(MctsNode*);

private:
	double timelimit;
	MctsNode* best_node;
	State* best_state;
	clock_t begin_time;
	ActionEvaluator* evl;
	MctsNode* root;
	SearchStrategy* greedy;
	typedef std::pair<MctsNode*, double> node_pair;
	vector < node_pair>nodes;
	void firstSimulations(MctsNode*,double,double);
	void reevaluation_of_nodes();

	/* variables:
	 * timer
	 * iteraciones
	 * maximo numero de iteraciones
	 * maximo numero de milisegundos
	 * profundidad de simulacion
	 * estimador de regresion lineal e intervalos de confianza
	 * */


	/*funciones
	 *
	 * constructor -> listo?
	 * mejor hijo inmediato (segun puntaje) //recorre los hijos, retorna el mejor hijo
	 * hijo mas visitado
	 * run: loop principal: seleccion, expansion, simulacion
	 *
	 *
	 * */

};

} /* namespace std */

#endif /* MCTS_H_ */
