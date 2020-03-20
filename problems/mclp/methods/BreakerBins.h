#ifndef BREAKERBINS_H
#define BREAKERBINS_H

#include <list>
#include <set>
#include <iostream>

using namespace std;

namespace mclp{
  class BreakerBins{
    private:
      pair< double,double > computeExclusiveVolumen(list<set<int>>& total_bins, int id_bin);
      pair< double,list<set<int>>::iterator > minExclusiveVolume(list<set<int>>& bins);
      pair< double,list<set<int>>::iterator > minLargestExlusiveBox(list<set<int>>& bins);


    public:
      BreakerBins();
      list < set<int> > get_break_bins(list < set<int> > best_bins, set <int>& break_boxes, double break_value,double prob = 0.5);
      list < set<int> > get_break_bins_random(list < set<int> > best_bins, set <int>& break_boxes, double break_value);
    };
}


#endif
