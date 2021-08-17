#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "objects2/BoxShape.h"
#include "clpState.h"

using namespace std;
using namespace metasolver;

namespace clp {

	//debería estar en AABB
     list<const AABB*> clpState::get_adjacent_aabbs(const AABB& ab, int adj, int d) const{

		Vector3 mins = ab.getMins() + Vector3(1,1,1);
		Vector3 maxs = ab.getMaxs() - Vector3(1,1,1);
		for (int nbit=0; nbit<6; nbit++){
			int bit = (adj & ( 1 << nbit )) >> nbit;
			if(bit == 1){
				 switch(nbit){
					 case 0: //DOWN
					   mins.setZ(std::max(ab.getMins().getZ()-d,(long)0));  break;
					 case 1: //UP
		 			   maxs.setZ(ab.getMaxs().getZ()+d); break;
					 case 2: //BACK
					   mins.setX(std::max(ab.getMins().getX()-d,(long)0)); break;
					 case 3: //FORTH
		 			   maxs.setX(ab.getMaxs().getX()+d); break;
					 case 4: //LEFT
					   mins.setY(std::max(ab.getMins().getY()-d, (long)0)); break;
					 case 5: //RIGHT
		 			   maxs.setY(ab.getMaxs().getY()+d); break;
				 }
			}
		}

		list<const AABB*> aabb_list=cont->blocks->get_intersected_objects(AABB(mins,maxs));
		
		return aabb_list;
	}

    /*void clpState::get_adjacent_aabbs(const AABB& ab, list<const AABB*>& aabb_list, int adj, int d) const{
		Vector3 mins = ab.getMins() + Vector3(1,1,1);
		Vector3 maxs = ab.getMaxs() - Vector3(1,1,1);
		for (int nbit=0; nbit<6; nbit++){
			int bit = (adj & ( 1 << nbit )) >> nbit;
			if(bit == 1){
				 switch(nbit){
					 case 0: //DOWN
					   mins.setZ(std::max(ab.getMins().getZ()-d,(long)0));  break;
					 case 1: //UP
		 			   maxs.setZ(ab.getMaxs().getZ()+d); break;
					 case 2: //BACK
					   mins.setX(std::max(ab.getMins().getX()-d,(long)0)); break;
					 case 3: //FORTH
		 			   maxs.setX(ab.getMaxs().getX()+d); break;
					 case 4: //LEFT
					   mins.setY(std::max(ab.getMins().getY()-d, (long)0)); break;
					 case 5: //RIGHT
		 			   maxs.setY(ab.getMaxs().getY()+d); break;
				 }
			}
		}

		aabb_list=singlebox_blocks->get_intersected_objects(AABB(mins,maxs));
	}*/

	

	/*double clpState::completeshipment() const{
		double CS = 0.000000000000001;
		double sumW = 0.0;
		map <int,int> types_of_solutionboxes;
		for(AABB aabb = singlebox_blocks->top(); singlebox_blocks->has_next(); aabb = singlebox_blocks->next()){
			types_of_solutionboxes[aabb.getBlock()->get_box_shape()->get_type()] += 1; //contando cuantas hay de cada tipo en la solucion actual
		}

		for(int j=0;j<7;j++){
			if(clpState::nb_boxes_by_type[j] != 0){
				
				double w = (1/pow((double) clpState::nb_boxes_by_type[j],2));
				sumW +=w;
				if(clpState::nb_boxes_by_type[j]==types_of_solutionboxes[j]){
					CS += w;
				}
			}
		}

		//(1-(CS/sumW)); penalty 1 significa que no hay ninguno completo y 0 totalmente completo, es un % de grupos incompletos

		return (CS/sumW);
	}*/

}