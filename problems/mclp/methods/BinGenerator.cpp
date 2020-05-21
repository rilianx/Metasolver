#include "BinGenerator.h"


namespace mclp{
  BinGenerator::BinGenerator(){
	  cout << "Generando conjunto de bins" << endl;
  };

  set<set<int>> BinGenerator::generate_bins(SearchStrategy* clp_solver, mclpState* s0, set<int>& id_boxes, int nbins, pair<double,double> limits){
    typedef set<int> Bin;
    set < Bin > bins;
  	map <int, int> used_boxes;
  	Bin new_bin;
	double min = limits.first;
	double max = min + limits.second;
	if(max > 1) max = 1.0;
	cout << max << endl;

  	for(auto box: s0->nb_left_boxes){
  		if(id_boxes.find(box.first->get_id())!=id_boxes.end()) used_boxes[box.first->get_id()] = 0;
  		else used_boxes[box.first->get_id()] = 20;
  	}

  	for(auto b : s0->nb_left_boxes){
		b.first->set_profit(b.first->getVolume()*random(min, max));
	}

  	int nb_boxes=0;

  	for(int i=0; i < nbins || (id_boxes.size() > nb_boxes); i++){
  		bool insert_bin = true;
  		//copia el estado base
  		mclpState& s_copy= *dynamic_cast<mclpState*>(s0->clone());
  		//usa clp_solver para llenar contenedor
  		double eval = clp_solver->run(s_copy);
  		const mclpState* best_state=dynamic_cast<const mclpState*>(clp_solver->get_best_state());

		for(auto box: dynamic_cast<const mclpState*>(clp_solver->get_best_state())->cont->nb_boxes){
  			new_bin.insert(box.first->get_id());
  			if(used_boxes[box.first->get_id()]==0) nb_boxes++;
  			used_boxes[box.first->get_id()]++;

			box.first->set_profit(box.first->get_profit()*random(min, max));
  		}

  		//se busca el nuevo bin en el conjunto de bins creados
  		for(auto bin: bins){
  			if(new_bin == bin){
  				insert_bin = false;
  				break;
  			}
  		}
  		if(insert_bin && !new_bin.empty()){
  			bins.insert(new_bin);
  		}
  		new_bin.clear();
  		//delete &s_copy;
  	}

  	//cout << "nb_boxes:" << nb_boxes << endl;
  	return bins;
  }
}
