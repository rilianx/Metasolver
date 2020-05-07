#include "Group_bin.h"
#include "mclp-state.h"
namespace mclp{

  Group_bin::Group_bin(int id, set<set<int>>* bins){
      id_group = id;
      set_bins = *bins;
      prom_metrica = 1;
      size_group = bins->size();
  };

  set<int> Group_bin::get_bin(int id){
      int i = 0;
      for( auto bin: set_bins ){
          if(i == id) return bin;
          i++;
      }
      cout << "ERROR DE INDEX" << endl;
      //return nullptr;
  }

  bool Group_bin::compare_group(int id_to_com, list<Group_bin> universe){
      //FALSE -> NO TIENE CAJAS UNICAS
      //TRUE -> TIENE CAJAS UNICAS
      bool exclusive = false;
      for(auto group: universe){
          if(group.get_group() != id_group){
              bool unique = compare_with_bin(id_to_com, group.get_set_bins());
              if(unique) exclusive = true;
          }
      }
      return exclusive;
  }

  bool Group_bin::compare_with_bin(int id_bin, set<set<int>> set_bin){
      //False -> no tiene caja unica
      //True -> tiene una caja unica
      bool exclusive_box = false;
      set<int> bin_objetive = get_bin(id_bin);
      for(auto id_box: bin_objetive){
          for(auto eval_bin: set_bin){
            if (eval_bin.count(id_box) == 0) exclusive_box = true;
          }
      }
      return exclusive_box;
  }



  void Group_bin::remove_bin(set<int>* bins){
    
    int eliminates = 0; 
    set<set<int>>* new_bin;
    new_bin = new(set<set<int>>);
    int index_id = 0;

    for(auto bin: set_bins){
        if(bins->count(index_id) == 0){
            new_bin->insert(bin);
        }else eliminates++;
        index_id++;
    }
    
    if(eliminates != 0) cout << "SE ELIMINARON "<< eliminates << " DEL GRUPO " << get_group() << endl;
    else cout << "NO HUBO ELIMINACION " << bins->size() << endl;
    
    set_bins = *new_bin;
    size_group = set_bins.size();
    delete(new_bin);
    
  }
    
  set<set<int>> Group_bin::get_set_bins(){
      return set_bins;
  }
  
  int Group_bin::get_totalBins(){
      return size_group;
  }

  int Group_bin::get_group(){
      return id_group;
  }

}
