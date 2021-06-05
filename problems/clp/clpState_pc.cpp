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
     list<AABB> clpState::get_adjacent_aabbs(const AABB& ab, int adj, int d) const{

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
		list<AABB> adjacent_bloxs;

		list<const AABB*> aabb_list=cont->blocks->get_intersected_objects(AABB(mins,maxs));
		for (auto aabb: aabb_list){
			if(!(ab <= *aabb)) //is not contained in the aabb-block
				for(auto bloxs:aabb->getBlock()->aabb_bloxs){
					AABB ab2 = AABB(aabb->getMins() + bloxs.getMins(), bloxs.getBlock());
					if(ab.intersects(ab2))
						adjacent_bloxs.push_back(ab);
				}
		}
		
		return adjacent_bloxs;
	}

    void clpState::get_adjacent_aabbs(const AABB& ab, list<const AABB*>& aabb_list, int adj, int d) const{
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
	}

	map <int, set<string>> clpState::recursive_extra_movements(AABB aabb, int client, map <int, set<string>> extra_movementsMAP) const{
		//creo interseccion con plano superior de caja aabb
		string key = std::to_string(aabb.getXmax())+std::to_string(aabb.getYmax())+std::to_string(aabb.getZmax());
		if((int)aabb.getBlock()->get_box_shape()->get_type() > client){
			set<string> set_aux = extra_movementsMAP[client]; 
			set_aux.insert(key);
			extra_movementsMAP[client] = set_aux;
		}
		//cajas superiores
		list<const AABB*> inter = singlebox_blocks->get_intersected_objects(AABB(aabb.getXmin(),aabb.getYmin(),aabb.getZmax(),aabb.getXmax(),aabb.getYmax(),aabb.getZmax()));
		map<string,double>::iterator it;
		for(const AABB* aabbS : inter){
			//calculo el area de contacto
			if(aabb.getZmax() == aabbS->getZmin()){
				double area_aux = abs((min(aabbS->getXmax(),aabb.getXmax()) - max( aabbS->getXmin(), aabb.getXmin()))) * abs((min( aabbS->getYmax(), aabb.getYmax()) - max( aabbS->getYmin(), aabb.getYmin())));
				if(area_aux>0){
					recursive_extra_movements(*aabbS, client,extra_movementsMAP);

				}
			}
		}
		//cajas frontales
		list<const AABB*> interF = singlebox_blocks->get_intersected_objects(AABB(aabb.getXmin(),aabb.getYmax(),aabb.getZmin(),aabb.getXmax(),aabb.getYmax(),aabb.getZmax()));
		map<string,double>::iterator itF;
		for(const AABB* aabbF : interF){
			//calculo el area de contacto
			if(aabb.getYmax() == aabbF->getYmin()){
				double area_aux2 = abs((min(aabbF->getXmax(),aabb.getXmax()) - max( aabbF->getXmin(), aabb.getXmin()))) * abs((min( aabbF->getZmax(), aabb.getZmax()) - max( aabbF->getZmin(), aabb.getZmin())));
				if(area_aux2>0){
					recursive_extra_movements(*aabbF, client,extra_movementsMAP);

				}
			}
		}
		return extra_movementsMAP;
	}

	double clpState::multidrop() const{ 

		list<const AABB*> aabbs;
		map <int, set<string>> extra_movementsMAP;

		for(AABB aabb = singlebox_blocks->top(); singlebox_blocks->has_next(); aabb = singlebox_blocks->next()){
			//cout << "Esta caja esta en X: " << aabb.getBlock()->getX()  << '\n';
			int client = aabb.getBlock()->get_box_shape()->get_type();

			//lista de cajas al frente 
			list<const AABB*> aabb_listFront;
			//todas las cajas al frente a una distancia desde la caja hasta lo mas al frente
			get_adjacent_aabbs(AABB(aabb.getXmin(),aabb.getYmin(),aabb.getZmin(),aabb.getXmax(),aabb.getYmax(),aabb.getZmax()), aabb_listFront, clpState::FORTH, cont->getL()-aabb.getYmax());

			//lista de cajas arriba
			list<const AABB*> aabb_listUP;
			//todas las cajas al frente a una distancia desde la caja hasta lo mas al frente
			get_adjacent_aabbs(AABB(aabb.getXmin(),aabb.getYmin(),aabb.getZmin(),aabb.getXmax(),aabb.getYmax(),aabb.getZmax()), aabb_listUP, clpState::UP, cont->getH()-aabb.getZmax());
			aabb_listFront.merge(aabb_listUP); //mezclar listas


			for(const AABB* aabbA : aabb_listFront){
				int thisClient = aabbA->getBlock()->get_box_shape()->get_type();
				if(thisClient>=client){
					extra_movementsMAP = recursive_extra_movements(*aabbA,client,extra_movementsMAP);
				}

			}
		}

		int total_extra = 0;
		for (map<int,set<string>>::iterator it=extra_movementsMAP.begin(); it!=extra_movementsMAP.end(); ++it){
			//cout << "Boxes of client " << it->first << " needs " << it->second.size() << " extra movements to remove them " << '\n';
			total_extra +=it->second.size();
		}
		//cout << "In total " <<total_extra << " extra movements needs to be done to remove all the boxes" << '\n';
		return total_extra;
	}


	//lite version
	double clpState::loadbalance() const{

		double distance = 0;
		double CM_Y = 0;
		double CM_Z = 0;
		//mitad del ancho y alto = peor distancia posible

		double total_weight = 0.00000000000000001;
		for(AABB aabb = singlebox_blocks->top(); singlebox_blocks->has_next(); aabb = singlebox_blocks->next()){

			CM_Y += ((double)aabb.getYmax() - ((double)aabb.getW()/2)) * aabb.getBlock()->get_box_shape()->get_weight();
			CM_Z += ((double)aabb.getZmax() - ((double)aabb.getH()/2)) * aabb.getBlock()->get_box_shape()->get_weight();
			total_weight+=aabb.getBlock()->get_box_shape()->get_weight();
		}
		CM_Y = CM_Y/total_weight;
		CM_Z = CM_Z/total_weight;
		distance = (sqrt(pow ((cont->getW()/2-CM_Y),2)+pow((0-CM_Z),2))); //distancia desde el suelo y mitad del contenedor al CM de la solucion
		//cout << "Center of mass of the solution: " << CM_Y << "," << CM_Z << '\n'; //posicion del CM de la solucion
		//cout << "Euclidean distance to solution CM from the container best possible CM: " << distance << '\n';

		return distance;
	}

	//pro version
	double clpState::loadbalanceA() const{
		double a = 0.0;

		double D = 0;
		double CM_Y = 0;
		double CM_Z = 0;
		double bestA = 1.41372;
		//mitad del ancho y alto = peor distancia posible

		double total_weight = 0.00000000000000001;
		for(AABB aabb = singlebox_blocks->top(); singlebox_blocks->has_next(); aabb = singlebox_blocks->next()){

			CM_Y += ((double)aabb.getYmax() - ((double)aabb.getW()/2)) * aabb.getBlock()->get_box_shape()->get_weight();
			CM_Z += ((double)aabb.getZmax() - ((double)aabb.getH()/2)) * aabb.getBlock()->get_box_shape()->get_weight();
			total_weight+=aabb.getBlock()->get_box_shape()->get_weight();
		}
		CM_Y = CM_Y/total_weight;
		CM_Z = CM_Z/total_weight;

		if(CM_Y>cont->getW()/2){
			CM_Y = cont->getW()-CM_Y;
		}

		D = sqrt(pow(CM_Y,2)+pow(CM_Z,2)); 
		//cout << "D: " << D << '\n'; 

		//cout << "D : " << D << '\n';
		if(0==D){
			return bestA;
		} 

		a = acos(CM_Z/D);
		//cout << "a: " << a << '\n'; 
		//a esta en radianes
		return a+0.000000000000000000000001;
	}

	double clpState::completeshipment() const{
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
	}

}