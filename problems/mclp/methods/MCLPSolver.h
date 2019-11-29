#ifndef MCLPSOLVER_H
#define MCLPSOLVER_H

#include "SetCovSolver.h"
#include "BinGenerator.h"
#include "Greedy.h"
#include <iostream>
#include <fstream>
#include <unistd.h>

namespace mclp{
  class MCLPSolver{
    private:
      string gurobi_path;
      int solver_iter;
      double break_value;
      int nbins;
      int first_iter;

    public:
      MCLPSolver(string gurobi_dir, int iter, double break_val, int n_bins=1000);
      int solve(Greedy* gr, BSG *bsg, mclpState* s0, double pdec);
      int getfirstbins();
  };
}
#endif
