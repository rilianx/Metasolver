#ifndef SETCOVSOLVER_H
#define SETCOVSOLVER_H

#include "BinGenerator.h"
#include "Greedy.h"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include "BSG.h"
#include <string>
#include <dirent.h>

namespace mclp{
  class SetCovSolver{
    private:
      string gurobi_path;
      string findDirectory(string current_directory, string find_directory);
      string findOut(string current_directory, string find_directory);
      string findInto(string current_directory, string find_directory);
      void exportToTxtSCP(set < set<int> >* bins, long int nb_boxes, set<int> boxes);

    public:
      SetCovSolver(string gurobi_dir);
      void solve_set_covering(set < set<int> >& bins, set<int> boxes , int nb_box_types);
  };
}
#endif
