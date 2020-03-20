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
      int first_sol;
      int lastupdate;
      int lastbestsize;
      mclpState *s0;
      map<int, list< pair<int,int> > > id_bin;

    public:
      MCLPSolver(string gurobi_dir, int iter, double break_val, int n_bins, mclpState *s_0);
      int solve(SearchStrategy *solver, double pdec, double prob, pair<double,double> limits);
      int solver(SearchStrategy *solver, double pdec, double prob, pair<double,double> limits);
      void exportToTxtSCP(set<set<set<int>>>* total_bins, int nt_bins);
      double calculate_percent(set<int>* first_set, set<int> *second_set, mclpState* s0);
      int getGlobalId(int id_local = 0, int group = 0);
      int getfirstbins();
      int getfirstSol();
      int getlastUpdate();
  };
}
#endif
