#include "BreakerBins.h"
#include "mclp-state.h"
namespace mclp{
  BreakerBins::BreakerBins(){};
  
  pair<double,double> BreakerBins::computeExclusiveVolumen(list<set<int>>& total_bins, int id_bin){
  	int i = 0;
  	int j = 0;
  	double vol = 0;
  	double max_vol = 0;
  	set<int> id_boxes;
  	for(auto bin: total_bins){
  		if(i == id_bin) id_boxes = bin;
  		i++;
  	}
  	for(auto box: id_boxes)
  	{
  		bool exclusive = true;
  		double volu_box = 0;
  		int j = 0;
  		for(auto bin: total_bins)
  		{
  			if(j != id_bin)
  			{
  				if(bin.count(box) > 0) exclusive = false;
  			}
  			j++;
  		}
  		if(exclusive){
  			double volu_box = mclpState::id2box[box]->getVolume();
  			if(volu_box > max_vol) max_vol = volu_box;
  			vol += volu_box;
  		}
  	}
  	return make_pair(vol,max_vol);
  }

  pair<double,list< set<int> >::iterator> BreakerBins::minExclusiveVolume(list<set<int>>& bins){
  	//list<set<int>> new_list_bin;
  	double minvolume =  std::numeric_limits<double>::max();
  	int id_bin = 0;
  	int id_actual_min = 0;
  	list< set<int> >::iterator min_bin;
  	//cout << "############################################"<< endl;
  	for(auto bin=bins.begin(); bin!= bins.end(); bin++, id_bin++ ){
  		double exc_vol = computeExclusiveVolumen(bins, id_bin).first;
  		if(exc_vol < minvolume){
  			minvolume = exc_vol;
  			min_bin = bin;
  			id_actual_min = id_bin;
  			//cout << minvolume << endl;
  		}
  	}

  	//cout << minvolume << endl;
  	/*id_bin = 0;
  	for(auto bin: bins){
  		if(id_bin != id_actual_min){
  			new_list_bin.push_back(bin);
  		}
  	}*/

  	pair<double,list< set<int> >::iterator  > minimum;
  	minimum.first = minvolume;
  	minimum.second = min_bin;
  	return minimum;
  }

  pair<double,list< set<int> >::iterator > BreakerBins::minLargestExlusiveBox(list<set<int>>& bins){
  	//list<set<int>> new_list_bin;
  	double minvolume =  std::numeric_limits<double>::max();
  	int id_bin = 0;
  	int id_actual_min = 0;
  	list< set<int> >::iterator min_bin;
  	for(auto bin=bins.begin(); bin!= bins.end(); bin++, id_bin++ ){
  		double exc_vol = computeExclusiveVolumen(bins, id_bin).second;

  		if(exc_vol < minvolume){
  			minvolume = exc_vol;
  			min_bin = bin;
  			id_actual_min = id_bin;
  		}
  	}
  	/*id_bin = 0;
  	for(auto bin: bins){
  		if(id_bin != id_actual_min){
  			new_list_bin.push_back(bin);
  		}
  	}*/

  	pair<double,list< set<int> >::iterator > minimum;
  	minimum.first = minvolume;
  	minimum.second = min_bin;
  	return minimum;
  }


  list < set<int> > BreakerBins::get_break_bins(list < set<int> > best_bins, set <int>& break_boxes, double break_value){
  	//para obtener una caja usando el id
  	//BoxShape* boxt = mclpState::id2box[1];
  	list < set <int> > break_bins;
  	int n = best_bins.size()*break_value;
  	for(int i=0; i<n; i++){
  		list < set<int> >::iterator it;
  		if(i%2==0)
  		   it = minExclusiveVolume(best_bins).second;
  		else
  		   it = minLargestExlusiveBox(best_bins).second;
  		break_bins.push_back(*it);

  		for (auto box: *it)
  			break_boxes.insert(box);

  		best_bins.erase(it);
  	}

  	for (set<int> bin: best_bins){
  		for(int box: bin){
  			if(break_boxes.find(box)!=break_boxes.end()) break_boxes.erase(box);
  		}
  	}
  	return break_bins;
  }

  list < set<int> > BreakerBins::get_break_bins_random(list < set<int> > best_bins, set <int>& break_boxes, double break_value){
  	//para obtener una caja usando el id
  	//BoxShape* boxt = mclpState::id2box[1];
  	//cout << boxt->getVolume() << endl;
  	list < set <int> > break_bins;
  	int n = best_bins.size()*break_value;
  	for(int i=0; i<n; i++){
  		int r=rand()%best_bins.size();
  		list < set<int> >::iterator it = best_bins.begin();
  		std::advance(it,r);
  		break_bins.push_back(*it);

  		for (auto box: *it)
  			break_boxes.insert(box);

  		best_bins.erase(it);
  	}
  	for (set<int> bin: best_bins){
  		for(int box: bin){
  			if(break_boxes.find(box)!=break_boxes.end()){
  				break_boxes.erase(box);
  			}
  		}
  	}
  	return break_bins;
  }
}
