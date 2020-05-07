#include "MCLPSolver.h"
#include "SetCovSolver.h"
#include "BinGenerator.h"
#include "BreakerBins.h"
#include "Greedy.h"

using namespace std;

namespace mclp{

  MCLPSolver::MCLPSolver(string gurobi_dir, int iter, double break_val, int n_bins, int ngroups , mclpState *s_0, double lim){
    gurobi_path = gurobi_dir;
    solver_iter = iter;
    break_value = break_val;
    nbins = n_bins;
    first_iter = 0;
    first_sol = 0;
    lastupdate = 0;
    lastbestsize = 0;
    s0 = s_0;
    n_groups = ngroups;
    limit_metric = lim;
  }

  int MCLPSolver::solve(SearchStrategy* solver, double pdec, double prob, pair<double,double> limits){
  	SearchStrategy *clp_solver = solver;
  	set<int> boxes;
  	
    for(auto box: s0->nb_left_boxes)
  		boxes.insert( box.first->get_id() );
  	
    set < set<int> > best_bins;
  	//set < set<int> > break_bins;
  	set < set<int> > bins;
  	bool first = true;
    
  	for(int i=0;i<solver_iter;i++){
      BinGenerator *generador;
      generador = new BinGenerator();
      bins = generador->generate_bins(clp_solver, s0, boxes, nbins, limits);
  	  
      if( i==0 ) {
        first_iter=bins.size();
        lastbestsize=best_bins.size();
       }
  	  int cant_boxes = s0->nb_left_boxes.size();
      //resuelve set-covering y deja los bins de la solucion optima
      SetCovSolver *CovSolver;
      CovSolver = new SetCovSolver(gurobi_path);
      CovSolver->solve_set_covering(bins, boxes, cant_boxes);

      if(i == 0) 
        first_sol = bins.size();
  	  
      set <int> aux_boxes;
  	  
      for(auto bin:bins){
  		  aux_boxes.insert(bin.begin(),bin.end());
  	  }
  		//cout << "boxes in gurobi solution: " << aux_boxes.size() << endl;
  		if(best_bins.empty()) {
        best_bins=bins;
        lastbestsize = bins.size();
        lastupdate = i;
      }else if(bins.size() < best_bins.size())
        best_bins=bins;
      
      
      
      /*else //if(bins.size() < break_bins.size()){
  			//best_bins <- best_bins - break_bins + bins
  			//for (auto bin:break_bins)
  			//	best_bins.erase(bin);

  			for (auto bin:bins)
  				best_bins.insert(bin);
  		}*/
  		boxes.clear();
      //BreakerBins *destroyer;
      //destroyer = new BreakerBins();
      //break_bins = destroyer->get_break_bins_random(best_bins, boxes, break_value);
  		//break_bins = destroyer->get_break_bins(best_bins, boxes, break_value, prob);
      if(best_bins.size()<lastbestsize) 
        lastupdate = i;
  		cout <<"Mejor solucion: " << best_bins.size() << endl;
      lastbestsize=best_bins.size();
  	}
  	return best_bins.size();
  }

  int MCLPSolver::solver(SearchStrategy* solver, double pdec, double prob ,pair<double,double> limits){
    SearchStrategy *clp_solver = solver;
    set<int> boxes;
    int nbins_iter = nbins/n_groups;
    for(auto box: s0->nb_left_boxes)
      boxes.insert(box.first->get_id());
    
    int boxes_to_gur = 0;
    int cant_boxes = s0->nb_left_boxes.size();
    set<set<int>> best_bins;
    set<set<int>> bins;
    
    //Se empiezan a generar los distintos subconjuntos de bins
    for(int j = 0; j < n_groups; j++){  
      set<set<int>> *aux_bins;
      aux_bins = new set<set<int>>;
      
      //Se genera una fraccion de bins, se aplica greedy
      BinGenerator *generador;
      generador = new BinGenerator();
      *aux_bins = generador->generate_bins(clp_solver, s0, boxes, nbins_iter, limits);
      cout << "BINS GENERADOS " << aux_bins->size() << endl;
         
      //Aplicar filtro de bins
      set<int> to_erase;
      int counter = 0;
      for(auto bin: *aux_bins){
        if(combin_touniver(bin)){
          to_erase.insert(counter);
        } 
        counter++;
      }

      
      int i = 0;
      set<set<int>> aux_group;
      for(auto bin: *aux_bins){
        if(to_erase.count(i) == 0)
          aux_group.insert(bin);   
        else
          if(find_unique(*aux_bins,i,bin)) 
            aux_group.insert(bin);
        i++;
      }

      *aux_bins = aux_group;
      cout << "BINS CON REDUCCIÓN " << aux_bins->size() << endl;
      
      //Aplicacion de SetCovSolver (Greedy)        
      SetCovSolver *CovSolver; 
      CovSolver = new SetCovSolver( gurobi_path );
      CovSolver->solve_set_covering( *aux_bins, boxes, cant_boxes );
      
      //Se genera un nuevo grupo y se inserta
      Group_bin *new_group;
      new_group = new Group_bin( j, aux_bins );
      bin_by_group.push_back( *new_group );
      
      
      //Cajas totales a darle a gurobi
      //Todos los bins se agregan a un universo de bins optimos
      boxes_to_gur += aux_bins->size();
      delete aux_bins;
      delete new_group;
    }

    NombrateID();

    
    setpair_bin_value();

    /*Aqui se debe aplicar la metrica*/
    double valor_metrica = calculateMetric(boxes_to_gur);

    NombrateID();

    exportToTxtSCP(boxes_to_gur); 

    for(auto group: bin_by_group){
      set<set<int>> bins_of_the_group = group.get_set_bins();
      for(auto b: bins_of_the_group){
        bins.insert(b);
      }
    }

    //Se aplica el gurobi final al conjunto completo
    SetCovSolver *CovSolver;
    CovSolver = new SetCovSolver(gurobi_path);
    CovSolver->solve_set_covering(bins, boxes, cant_boxes);
    best_bins = bins;
    return best_bins.size();
  };



  //Calcula el valor de SET< < id_global_1,id_global_2 > ,percent > para todo un universo de bins
  void MCLPSolver::setpair_bin_value(){
    for(auto group: bin_by_group){
      int len_group = group.get_totalBins();
      group_sizes.push_back(len_group);
    }   
    //*Comparacion de un bin de un grupo con los bins de otros grupos*/
    set<int> recorridos;
    //Se toma un grupo
    for ( auto fgroup: bin_by_group ){
        //Se elige un grupo distinto para comprobar
        for( auto sgroup: bin_by_group ){
          //Se verifica que los conjuntos sean distintos
          if( fgroup.get_group() != sgroup.get_group() ){
            //Y que no se hayan recorrido anteriormente
            if( recorridos.count( sgroup.get_group() ) == 0 ){
              //Se elige el conjunto de bins de cada grupo
              set<set<int>> f_set = fgroup.get_set_bins();
              set<set<int>> s_set = sgroup.get_set_bins();
              int id_fbin = 0;
              //Se comparan todos los bins de 1 y 2.
              for( auto first_bin_first :f_set ){
                int id_sbin = 0;
                for( auto first_bin_second : s_set ){
                  int f_bin = getGlobalId( id_fbin, fgroup.get_group() );
                  int s_bin = getGlobalId( id_sbin, sgroup.get_group() );
                  //Se compara el grado de similitud de ambos conjuntos
                  double value = percent_from_b2b( &first_bin_first, &first_bin_second );
                  setfillpairbin( f_bin, s_bin, value );
                  id_sbin++;
                }
                id_fbin++;
              };
            }
          }
        }
        recorridos.insert(fgroup.get_group());
      }      
  }


  //Segmento de codigo utilizado para calcular la relacion existente entre cada bin de cada grupo
  //lista( <id dentro del grupo, id dentro del universo> ) 
  void MCLPSolver::NombrateID(){
    map<int, list< pair<int,int>>> *new_id;
    new_id = new (map<int, list< pair<int,int>>>);
    int global_id = 0;
    for( auto actual_group : bin_by_group ){
      list< pair<int,int> > pares_id;
      for(int i = 0; i < actual_group.get_totalBins(); i++ ){
        pair<int,int> insert_bin;
        insert_bin.first = global_id;
        insert_bin.second = i;
        pares_id.push_back( insert_bin );
        global_id++;
      }
      new_id->insert( {actual_group.get_group() ,pares_id} );
    };
    id_bin = *new_id;
    delete(new_id);
  };


  void MCLPSolver::exportToTxtSCP(int nt_bins){
  	string filename = "set_bin" + to_string(getpid()) + ".txt";
    string path = "";
  	cout << "Resultados almacenados en " << path << filename << endl;
  	ofstream scp (path + filename);
  	if (scp.is_open()){
      scp << nt_bins << " " << n_groups <<"\n";
      int group_x = 0;

      /*Cantidad de elementos por grupo de bins*/
      for (auto actual_set: group_sizes){
        scp << group_x << " " << actual_set << "\n";
        group_x++;
      }
      for (auto par: pair_bin_value){
          pair<pair<int,int>,double> to_evalue;
          pair<int,int> bin_to_eval = par.first;
          scp << bin_to_eval.first << " " <<bin_to_eval.second << " " << par.second << endl;
      }
  		scp.close();
  	} else cout << "Unable to open file" << endl;
  }


  //Contiene el valor SET< <bin1,bin2>, pror > donde pror es la proporcion de ambos bin1 y bin2
  //set<pair<pair<int,int>,double>> pair_bin_value;  
  double MCLPSolver::calculateMetric(int n_bins){
    double similar_bins = 0.0;
    double value_metric = 0.0;
    set<int> *to_erase;
    to_erase = new(set<int>);

    //Se efectua el calculo y la identificacion de los bins que se deben eliminar
    for(auto val: pair_bin_value){
        double pair_val = val.second;
        if(pair_val >= limit_metric){
            int gfbin = (val.first).first;
            int gsbin = (val.first).second;
            if(to_erase->count(gfbin) == 0 || to_erase->count(gsbin)){
              pair<int,int> lfbin = getlocalId(gfbin);
              pair<int,int> lsbin = getlocalId(gsbin);
              for( auto group: bin_by_group ){
                if( group.get_group() == lfbin.first ){
                  if( group.compare_group (lfbin.second, bin_by_group) ){
                    to_erase->insert(gfbin);
                    similar_bins++;
                    break;
                  }
                }else if( group.get_group() == lsbin.first){   
                  if( group.compare_group (lsbin.second, bin_by_group) ){
                    to_erase->insert(gsbin);
                    similar_bins++;
                    break;
                  }
                }
              }
            }
        }      
    }
   
    //Se realiza la eliminacion de un set de bins mediante su id_local por cada grupo
    /*list<Group_bin>* new_group;
    new_group = new(list<Group_bin>);
    for(auto group: bin_by_group){
      set<int>* erase_by_group;
      erase_by_group = new(set<int>);
      for(auto bin_to_erase : *to_erase){
        pair<int,int> bin_eliminate = getlocalId(bin_to_erase);
          if(group.get_group() == bin_eliminate.first){
            erase_by_group->insert(bin_eliminate.second);      
          }
      }
      group.remove_bin(erase_by_group);
      new_group->push_back(group);
      delete(erase_by_group);
    }
    
    bin_by_group = *new_group;
    delete(to_erase);
    delete(new_group);
    */
    value_metric = similar_bins/n_bins;

    return value_metric;
  }
  
      
  bool MCLPSolver::combin_touniver(set<int> e_bin) {
    for(auto a_group: bin_by_group){
      set<set<int>> aux_group = a_group.get_set_bins();
      for(auto bin: aux_group){
        double val_metric = percent_from_b2b(&e_bin, &bin);
        //cout << "VALOR METRICA " << val_metric << endl;
        if(val_metric > limit_metric){          
          return true;
        } 
      }
    }
    return false;
  }



  double MCLPSolver::percent_from_b2b(set<int>* first_set, set<int>* second_set){
    double total_percent = 0.0;
    double val = 0.0;
    double total = 0.0;
    set<int> *union_bin;
    union_bin = new(set<int>);
    
    for(auto first_box: *first_set){
      if(second_set->count(first_box) != 0) 
        val += mclpState::id2box[first_box]->getVolume();
      union_bin->insert(first_box);
    }
    
    for(auto first_box: *union_bin) 
      total += mclpState::id2box[first_box]->getVolume();
    
    total_percent = val/total;
    return total_percent;
  }

  bool MCLPSolver::find_unique(set<set<int>> actual_group, int id_bin, set<int> e_bin){
    for(auto box: e_bin){
      int index = 0;
      bool unique_box = true;
      for(auto bin: actual_group){
        if(index != id_bin){
          if(bin.count(box) > 0) unique_box = false;  
        }
        index++;
      
      }
      if(unique_box) return true;
    }
    return false;
  } 


  
  int MCLPSolver::getGlobalId( int id_local, int group ){
    for (auto value : id_bin){
      if( value.first == group ){
        for(auto key: value.second){
          if(key.second == id_local) return key.first;
        }
      }
    }
    cout << "NO SE ENCONTRO EL ID GLOBAL" << endl;
    return 0;
  };



  pair<int,int> MCLPSolver::getlocalId( int id_global ){
    for (auto value : id_bin){
      for (auto key: value.second){
          if (key.first == id_global) return make_pair( value.first,key.second );  
      }
    }
    cout << "NO SE ENCONTRO EL PAR GRUPO - ID LOCAL" << endl;
    return make_pair(0,0);
  };



  void MCLPSolver::setfillpairbin(int f_bin, int s_bin, double val_comp){
    pair<int,int> bins_to_comp;
    bins_to_comp = make_pair(f_bin,s_bin);
    pair<pair<int,int>, double> comp_val (bins_to_comp,val_comp); 
    pair_bin_value.insert(comp_val);
  };


  

  int MCLPSolver::getfirstSol(){
    return first_sol;
  };

  int MCLPSolver::getlastUpdate(){
    return lastupdate;
  };

  int MCLPSolver::getfirstbins(){
    return first_iter;
  };
 
}
