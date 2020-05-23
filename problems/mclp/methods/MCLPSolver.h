#ifndef MCLPSOLVER_H
#define MCLPSOLVER_H

#include "SetCovSolver.h"
#include "BinGenerator.h"
#include "Greedy.h"
#include <iostream>
#include "Group_bin.h"
#include <fstream>
#include <unistd.h>

namespace mclp{
  class MCLPSolver{
    private:
      mclpState *s0;
      string gurobi_path;
      int solver_iter;
      double break_value;
      int nbins;
      int n_groups;
      int first_iter;
      int first_sol;
      int lastupdate;
      int lastbestsize;
      double limit_metric;
      double value_metric;

      //Mapa con llave el id del grupo y entrega una lista con todos los bins con su <id_local, id_global>
      map<int, list< pair<int,int>>> id_bin;

      //Contiene el valor SET< <bin1,bin2>, pror > donde pror es la proporcion de ambos bin1 y bin2
      set<pair<pair<int,int>,double>> pair_bin_value;  
      
      //Estructura que contiene los grupos de bins, mediante la clase Group_bin
      list<Group_bin> bin_by_group;
      
      //Estructura que contiene el size de cada grupo. Cada grupo está identificado mediante su index
      list<int> group_sizes;

    public:
      //Constructor del solver
      MCLPSolver(string gurobi_dir, int iter, double break_val, int n_bins, int ngroups, mclpState *s_0, double lim);
      
      //Solver para todo el conjunto de bins
      int solve(SearchStrategy *solver, double pdec, double prob, pair<double,double> limits);
      
      //Solver para generar grupos de bins
      int solver(SearchStrategy *solver, double pdec, double prob, pair<double,double> limits);

      bool combin_touniver(set<int> e_bin);
      
      //Se entrega el valor de SET< < id_global_1,id_global_2 > ,percent >
      void setpair_bin_value();
      
      //Corrobora que si el grupo de bins creado tiene una caja única
      bool is_unique(set<set<int>> actual_group, int id_bin, set<int> e_bin);

      void filter_bins(set<set<int>>* actual_group);
      
      //Exportar txt con valores de pair_bin_value
      void exportToTxtSCP(int nt_bins);      

      //Funcion que le otorga un id_local e id_global a cada bin de cada grupo.
      void NombrateID();

      //Calcula una metrica para un conjunto de bins
      double calculateMetric(int n_bins);

      //Calcula el grado de similitud o proporcion entre dos bins
      double percent_from_b2b(set<int>* first_set, set<int>*second_set);
      
      //Desde la posicion local y el grupo, retorna id_global de un bin
      int getGlobalId(int id_local = 0, int group = 0);

      //Desde la posicion global, retorna el grupo y el id_local de un bin
      pair<int,int> getlocalId(int id_global);
      
      int getfirstbins();

      //Agrega el valor < <f_bin,s_bin> , val_com > a la estructura pair_bin_value
      void setfillpairbin(int f_bin, int s_bin, double val_comp);

      int getfirstSol();
      
      int getlastUpdate();

      double getMetric();
      
  };
}
#endif
