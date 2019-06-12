/*
 * A_star.h
 *
 *  Created on: 24 may. 2018
 *      Author: iaraya
 */

#ifndef STRATEGIES_A_STAR_H_
#define STRATEGIES_A_STAR_H_


#include <SearchStrategy.h>
#include <State.h>
#include <queue>
#include <unordered_set>
#include <set>
#include <fstream>
#include "mctsNode.h"

namespace metasolver {




    class MCTS : public SearchStrategy {
        public:

    	MCTS(ActionEvaluator* evl, SearchStrategy& greedy) :
    		SearchStrategy(evl), greedy(greedy), s0(NULL){}

	    virtual ~MCTS() {}



	    void dfsPrintChild(mctsNode* node, ofstream& file);

	    void pointsToTxt(mctsNode* root, int it);

		/**
		 * Run the strategy and return the best found value
		 */
		 double run(State& s, double tl=99999.9, clock_t bt=clock());

		mctsNode* select_node();

		// performs a simulation and returns the corresponding child
		void simulate(mctsNode* n, const State* s0);

		// hace que el nodo se vuelva selectable, i.e., genera sus hijos (prechildren)
		// y lo inserta en la lista de nodos
		void make_selectable(mctsNode* n, const State* s0);

		//selectable nodos por nivel
		map<int, int> level2nodes;

		//selected nodos por nivel
		map<int, int> level2selectednodes;


		//se descartan nodos equivalentes
		static bool discard_equivalent;

		private:

    	const State* s0;

		SearchStrategy& greedy;

		//set< double > evals;
		map< int, set<mctsNode*, node_comp> > level_nodes; //almacena las evaluaciones del nivel


		//nodos seleccionables del arbol ordenados por promesa
		list<mctsNode*> nodes;

};



} /* namespace metasolver */

#endif /* STRATEGIES_A_STAR_H_ */
