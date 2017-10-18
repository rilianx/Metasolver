/*
 * NSGA2.cpp
 *
 *  Created on: 18 oct. 2017
 *      Author: iaraya
 */

#include "NSGA2.h"

namespace clp {


NSGA2::~NSGA2() {
	// TODO Auto-generated destructor stub
}

//Basarse en este algoritmo: http://www.cleveralgorithms.com/nature-inspired/evolution/nsga.html
virtual list<Chromosome*> NSGA2::next(list<Chromosome*>& G){

	list<Chromosome*> offspringC = crossover(G);

	list<Chromosome*> offspringM = mutation(G);

	//se insertan los nuevos inidividuos a la poblacion G
	G.insert(G.end(), offspringC.begin(), offspringC.end());

	G.insert(G.end(), offspringM.begin(), offspringM.end());


	//se seleccionan los mejores individios para la siguiente iteracion
	return selection(G);


}

} /* namespace clp */
