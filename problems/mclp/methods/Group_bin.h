#ifndef GROUP_BIN_H
#define GROUP_BIN_H

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
  class Group_bin{
    private:
      int id_group;
      
      set<set<int>> set_bins;
      
      double prom_metrica;
      
      int size_group;

    public:
      Group_bin(int id, set<set<int>>* bins);

      bool compare_with_bin(int id_bin, set<set<int>> set_bin);

      bool compare_group(int id_to_com, list<Group_bin> universe);

      bool compare_group(set<set<int>> e_bin);
      
      void remove_bin(set<int>* id_bin);  

      set<int> get_bin(int id);

      set<set<int>> get_set_bins();

      int get_totalBins();

      int get_group();
      

  };
}
 #endif