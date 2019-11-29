#include "MCLPSolver.h"
#include "SetCovSolver.h"
#include "BinGenerator.h"
#include "BreakerBins.h"
#include "Greedy.h"

using namespace std;

namespace mclp{

  MCLPSolver::MCLPSolver(string gurobi_dir, int iter, double break_val, int n_bins){
    gurobi_path = gurobi_dir;
    solver_iter = iter;
    break_value = break_val;
    nbins = n_bins;
  }

  int MCLPSolver::solve(Greedy* gr, BSG *bsg, mclpState* s0, double pdec){
  	SearchStrategy * clp_solver = gr;
  	set<int> boxes;
  	for(auto box: s0->nb_left_boxes)
  		boxes.insert(box.first->get_id());

  	list < set<int> > best_bins;
  	list < set<int> > break_bins;
  	list < set<int> > bins;
  	bool first=true;
  	for(int i=0;i<solver_iter;i++){
      BinGenerator *generador;
      generador = new BinGenerator();
      bins = generador->generate_bins(clp_solver, s0, boxes /*, (i==0)? 100:1000*/);
  		//bins = generate_bins(clp_solver, s0, boxes /*, (i==0)? 100:1000*/);
  		cout <<"Bins generados por BSG: " <<bins.size() << endl;
  		if(i==0) first_iter=bins.size();
  		int cant_boxes = s0->nb_left_boxes.size();


      //resuelve set-covering y deja los bins de la solucion optima
      SetCovSolver *CovSolver;
      CovSolver = new SetCovSolver(gurobi_path);
      CovSolver->solve_set_covering(bins, boxes, cant_boxes);
  		cout <<"Solución Gurobi (subproblema) :" << bins.size() << "/" << break_bins.size() << endl;
  		set <int> aux_boxes;
  		for(auto bin:bins){
  			aux_boxes.insert(bin.begin(),bin.end());
  		}
  		//cout << "boxes in gurobi solution: " << aux_boxes.size() << endl;

  		if(best_bins.empty()) best_bins=bins;
  		else if(bins.size() < break_bins.size()){
  			//best_bins <- best_bins - break_bins + bins
  			for (auto bin:break_bins)
  				best_bins.remove(bin);

  			for (auto bin:bins)
  				best_bins.push_back(bin);

  		}
  		boxes.clear();
      BreakerBins *destroyer;
      destroyer = new BreakerBins();
      break_bins = destroyer->get_break_bins_random(best_bins, boxes, break_value);
  		//break_bins = get_break_bins_random(best_bins, boxes, break_value);
  		cout <<"Mejor solución: " << best_bins.size() << endl;
  	}
  	return bins.size();
  }
  
  int MCLPSolver::getfirstbins(){
    return first_iter;
  };
}
