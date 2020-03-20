#include "MCLPSolver.h"
#include "SetCovSolver.h"
#include "BinGenerator.h"
#include "BreakerBins.h"
#include "Greedy.h"

using namespace std;

namespace mclp{

  MCLPSolver::MCLPSolver(string gurobi_dir, int iter, double break_val, int n_bins, mclpState *s_0){
    gurobi_path = gurobi_dir;
    solver_iter = iter;
    break_value = break_val;
    nbins = n_bins;
    first_iter = 0;
    first_sol = 0;
    lastupdate = 0;
    lastbestsize = 0;
    s0 = s_0;
  }

  int MCLPSolver::solve(SearchStrategy* solver, double pdec, double prob, pair<double,double> limits){
  	SearchStrategy *clp_solver = solver;
  	set<int> boxes;
  	for(auto box: s0->nb_left_boxes)
  		boxes.insert(box.first->get_id());
  	set < set<int> > best_bins;
  	set < set<int> > break_bins;
  	set < set<int> > bins;
  	bool first = true;
  	for(int i=0;i<solver_iter;i++){

      BinGenerator *generador;
      generador = new BinGenerator();
      bins = generador->generate_bins(clp_solver, s0, boxes, nbins, limits);
  		//bins = generate_bins(clp_solver, s0, boxes /*, (i==0)? 100:1000*/);
  		//cout <<"Bins generados por BSG: " << bins.size() << endl;
  		if(i==0) {
        first_iter=bins.size();
        lastbestsize=best_bins.size();
      }
  		int cant_boxes = s0->nb_left_boxes.size();
      //resuelve set-covering y deja los bins de la solucion optima
      SetCovSolver *CovSolver;
      CovSolver = new SetCovSolver(gurobi_path);
      CovSolver->solve_set_covering(bins, boxes, cant_boxes);

      if(i == 0) first_sol = bins.size();
  		set <int> aux_boxes;
  		for(auto bin:bins){
  			aux_boxes.insert(bin.begin(),bin.end());
  		}
  		//cout << "boxes in gurobi solution: " << aux_boxes.size() << endl;
  		if(best_bins.empty()) {
        best_bins=bins;
        lastbestsize = bins.size();
        lastupdate = i;
      }
  		else if(bins.size() < break_bins.size()){
  			//best_bins <- best_bins - break_bins + bins
  			for (auto bin:break_bins)
  				best_bins.erase(bin);

  			for (auto bin:bins)
  				best_bins.insert(bin);
  		}
  		boxes.clear();
      BreakerBins *destroyer;
      destroyer = new BreakerBins();
      //break_bins = destroyer->get_break_bins_random(best_bins, boxes, break_value);
  		//break_bins = destroyer->get_break_bins(best_bins, boxes, break_value, prob);
      if(best_bins.size()<lastbestsize) lastupdate = i;
  		cout <<"Mejor solucion: " << best_bins.size() << endl;
      lastbestsize=best_bins.size();
  	}
  	return best_bins.size();
  }

  int MCLPSolver::solver(SearchStrategy *solver, double pdec, double prob, pair<double,double> limits){
    int ncontainer = 8;
    SearchStrategy *clp_solver = solver;
    set<int> boxes;
    int nbins_iter = nbins*0.25;
    for(auto box: s0->nb_left_boxes)
      boxes.insert(box.first->get_id());
    int cant_boxes = s0->nb_left_boxes.size();
    set< set<int> > best_bins;
    set< set<int> > bins;
    set< set< set<int> > > total_bins;                                          //Lista que contendra los conjuntos calculados en cada iteracion
    for(int j = 0; j < ncontainer; j++){
      set<set<int>>aux_bins;
      BinGenerator *generador;
      generador = new BinGenerator();
      aux_bins = generador->generate_bins(clp_solver, s0, boxes, nbins_iter, limits);
      SetCovSolver *CovSolver;
      CovSolver = new SetCovSolver(gurobi_path);
      CovSolver->solve_set_covering(aux_bins, boxes, cant_boxes);
      total_bins.insert(aux_bins);                                              //Se agrega el conjunto sacado actualmente de bins
      for(auto bin_selected: aux_bins){
        bins.insert(bin_selected);                                              //Todos los bins se agregan a un conjunto global de bins
      }
    }
    //Segmento de codigo solamente utilizado para construir el grafo de interaccion por beam
    int b = 0;
    int global_id = 0;
    for(auto set: total_bins){
      int local_id = 0;
      list< pair<int,int> > pares_id;
      for(auto bin: set){
        pair<int,int> insert_bin;
        insert_bin.first= global_id;
        insert_bin.second= local_id;
        pares_id.push_back(insert_bin);
        global_id++;
        local_id++;
        //cout << b <<" " <<counter_bin <<" " <<set_bin << endl;
      }
      id_bin.insert({b,pares_id});
      b++;
    }
    for (auto value : id_bin){
      for(auto key: value.second){
          cout << value.first<< " "<< key.first << " " << key.second <<endl;
      }
    }

    exportToTxtSCP(&total_bins, global_id);
    SetCovSolver *CovSolver;
    CovSolver = new SetCovSolver(gurobi_path);
    CovSolver->solve_set_covering(bins, boxes, cant_boxes);
    if(best_bins.empty())
        best_bins=bins;
    else if(best_bins.size() > bins.size() )
        best_bins=bins;
    return best_bins.size();
  };


  void MCLPSolver::exportToTxtSCP(set<set<set<int>>>* total_bins, int nt_bins){
  	string filename = "set_bin" + to_string(getpid()) + ".txt";
    string path = "";
  	cout << "Resultados almacenados en " << path << filename << endl;
  	ofstream scp (path + filename);
  	if (scp.is_open()){
      cout << nt_bins <<" "<< total_bins->size() <<"\n";
      scp << nt_bins <<" "<< total_bins->size() <<"\n";
      int group_x = 0;
      /*Cantidad de elementos por grupo de bins*/
      for (auto actual_set: *total_bins){
        scp << group_x << " " << actual_set.size() << "\n";
        group_x++;
      }
      /*Comparacion de un bin de un grupo con los bins de otros grupos*/
      set<int> recorridos;
      group_x = 0;
      for (auto first_set: *total_bins){
        int group_y = 0;
        for(auto second_set: *total_bins){
          if(group_x != group_y){
            if(recorridos.count(group_y)==0){
              int id_fbin = 0;
              for(auto first_bin_first :first_set){
                int id_sbin = 0;
                for(auto first_bin_second : second_set){
                  double value = calculate_percent(&first_bin_first, &first_bin_second, s0);
                  scp << getGlobalId(id_fbin, group_x) << " "<< getGlobalId(id_sbin, group_y)  << " "<< value <<"\n";
                  id_sbin++;
                }
                id_fbin++;
              };
            }
          }
          group_y++;
        }
        recorridos.insert(group_x);
        group_x++;
      }
  		scp.close();
  	} else cout << "Unable to open file" << endl;
  }

  double MCLPSolver::calculate_percent(set<int>* first_set, set<int> *second_set, mclpState* s0){
    double total_percent = 0.0;
    double val = 0.0;
    double total = 0.0;
    set<int> union_bin;
    for(auto first_box: *first_set){
      if(second_set->count(first_box) != 0) val += mclpState::id2box[first_box]->getVolume();
      union_bin.insert(first_box);
    }

    for(auto first_box: union_bin) total += mclpState::id2box[first_box]->getVolume();
    total_percent = val/total;

    return total_percent;

  }


  int MCLPSolver::getGlobalId(int id_local, int group){
    for (auto value : id_bin){
      if( value.first == group ){
        for(auto key: value.second){
          if(key.second == id_local) return key.first;
            //cout << value.first<< " "<< key.first << " " << key.second <<endl;
        }
      }
    }
  }

  int MCLPSolver::getfirstbins(){
    return first_iter;
  };


  int MCLPSolver::getfirstSol(){
    return first_sol;
  }

  int MCLPSolver::getlastUpdate(){
    return lastupdate;
  }

}
