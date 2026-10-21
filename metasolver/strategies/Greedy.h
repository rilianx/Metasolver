/*
 * Greedy.h
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

#include <vector>
#include <deque>
#include <unordered_map>
#include "../ActionEvaluator.h"
#include "../SearchStrategy.h"


#ifndef GREEDY_H_
#define GREEDY_H_

using namespace std;

namespace metasolver {

/**
 * Busqueda greedy: aplica la mejor accion hasta que no queden acciones.
 *
 * Skip feature (L. Guesser, Alg. 3.12): si el estado implementa get_hash(),
 * se recuerdan los estados visitados por los greedy anteriores junto con las
 * acciones que siguieron. Al llegar a un estado ya visitado, el resto de la
 * solucion se reconstruye aplicando esas acciones, sin volver a evaluarlas.
 * El resultado es el mismo que sin memoria (el greedy es determinista).
 */
class Greedy : public SearchStrategy {
public:

	Greedy(ActionEvaluator* evl, size_t max_memory=5000000) : SearchStrategy(evl), max_memory(max_memory) { }

	virtual ~Greedy();

	virtual double run(State& s, double tl=99999.9, clock_t bt=clock());

	/**
	 * Performs an iteration of the strategy
	 */
	virtual list<State*> next(list<State*>& S) ;

private:

	//posicion en la secuencia de acciones de un greedy anterior
	struct Record {
		int run;
		int pos;
	};

	struct Run {
		vector<Action*> actions;
		//continuacion (cuando el greedy termino en un estado ya visitado)
		Record next;
	};

	void replay(Record rec, State& s);

	deque<Run> runs;
	unordered_map<uint64_t, Record> memory;

	//numero maximo de estados recordados
	size_t max_memory;
};


} /* namespace clp */

#endif /* GREEDY_H_ */
