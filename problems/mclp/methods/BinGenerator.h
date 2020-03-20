#ifndef BINGENERATOR_H
#define BINGENERATOR_H

#include <iostream>
#include <fstream>
#include <list>
#include <set>
#include <iostream>
#include "mclp-state.h"
#include "Greedy.h"
#include "stdlib.h"

using namespace std;

namespace mclp{
  class BinGenerator{
    private:
      double random(double l, double r) {
      	return l + ((double)rand()/RAND_MAX)*(r-l);
      }

    public:
      BinGenerator();
      set<set<int>> generate_bins(SearchStrategy* clp_solver, mclpState* s0, set<int>& id_boxes, int nbins, pair<double, double> limits);

  };
}
 #endif
