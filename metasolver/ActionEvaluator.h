/*
 * ActionEvaluator.h
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

#ifndef HEURISTIC_FUNCTION_H_
#define HEURISTIC_FUNCTION_H_

namespace metasolver{

/**
 * Evalua las acciones que se pueden aplicar a un estado para seleccionar
 * las mejores. Cada problema define su evaluador y el estado lo utiliza
 * en State::get_best_actions.
 */
class ActionEvaluator {
public:
	virtual ~ActionEvaluator() { }
};

}

#endif /* HEURISTIC_FUNCTION_H_ */
