/*
 * clpState.cpp
 *
 *  Created on: 4 jul. 2017
 *      Author: iaraya
 */

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "objects2/BoxShape.h"
#include "clpState.h"

using namespace std;
using namespace metasolver;

namespace clp {


//Utilizado para obtener la medida de uno de los lados del area intersectada
long clpState::place(long minAABB, long maxAABB, long minInter, long maxInter) const{
	//cout << "maxAABB: " << maxAABB << "\tminAABB: " << minAABB << "\tminInter: " << minInter << "\tmaxInter: " << maxInter << endl;
	if(maxAABB <= maxInter && minInter <= minAABB){
		//cout << "Fuera de bloque original: " << maxAABB - minAABB << endl;
		return maxAABB - minAABB;
	}
	if(maxAABB > maxInter && minInter > minAABB){
		//cout << "Dentro de bloque original: " << maxAABB - minAABB << endl;
		return maxInter - minInter;
	}
	if(maxAABB <= maxInter){
		//cout << "maxAABB menor a maxInter: " << maxAABB - minInter << endl;
		return maxAABB - minInter;
	}
	//if(maxAABB > maxInter){
	//cout << "maxAABB mayor a maxInter: " << maxInter - minAABB << endl;
	return maxInter - minAABB;
	//}
}

/**
 * Used to obtain a block symmetry. Exist eight symmetries: without axis(0), in axis x, y, z, xy, xz, yz, xyz
 * parameters block
 * return list
 */
/*
 * Symmetry 1:	null	= (getXmin, getXmax, getYmin, getYmax, getZmin, getZmax)
 * Symmetry 2:	eje z	= (getXmin, getXmax, getYmin, getYmax, getH - getZmin, getH - getZmax)
 * Symmetry 3:	eje y	= (getXmin, getXmax, getW - getYmin, getW - getYmax, getZmin, getZmax)
 * Symmetry 4:	eje yz	= (getXmin, getXmax, getW - getYmin, getW - getYmax, getH - getZmin, getH - getZmax)
 * Symmetry 5:	eje x	= (getL - getXmin, getL - getXmax, getYmin, getYmax, getZmin, getZmax)
 * Symmetry 6:	eje xz	= (getL - getXmin, getL - getXmax, getYmin, getYmax, getH - getZmin, getH - getZmax)
 * Symmetry 7:	eje xy	= (getL - getXmin, getL - getXmax, getW - getYmin, getW - getYmax, getZmin, getZmax)
 * Symmetry 8:	eje xyz	= (getL - getXmin, getL - getXmax, getW - getYmin, getW - getYmax, getH - getZmin, getH - getZmax)
 */
void clpState::symmetryVolume(AABB aabb, long* symmetry, int index) const{
	//int binary[3] = {0, 0, 0};
	//vector<long> symmetry(6);

	switch(index){
		case 0:
			symmetry[0] = aabb.getXmin();
			symmetry[1] = aabb.getXmax();
			symmetry[2] = aabb.getYmin();
			symmetry[3] = aabb.getYmax();
			symmetry[4] = aabb.getZmin();
			symmetry[5] = aabb.getZmax();
			break;
		case 1:
			symmetry[0] = aabb.getXmin();
			symmetry[1] = aabb.getXmax();
			symmetry[2] = aabb.getYmin();
			symmetry[3] = aabb.getYmax();
			symmetry[4] = aabb.getH() - aabb.getZmin();
			symmetry[5] = aabb.getH() - aabb.getZmax();
			break;
		case 2:
			symmetry[0] = aabb.getXmin();
			symmetry[1] = aabb.getXmax();
			symmetry[2] = aabb.getW() - aabb.getYmin();
			symmetry[3] = aabb.getW() - aabb.getYmax();
			symmetry[4] = aabb.getZmin();
			symmetry[5] = aabb.getZmax();
			break;
		case 3:
			symmetry[0] = aabb.getXmin();
			symmetry[1] = aabb.getXmax();
			symmetry[2] = aabb.getW() - aabb.getYmin();
			symmetry[3] = aabb.getW() - aabb.getYmax();
			symmetry[4] = aabb.getH() - aabb.getZmin();
			symmetry[5] = aabb.getH() - aabb.getZmax();
			break;
		case 4:
			symmetry[0] = aabb.getL() - aabb.getXmin();
			symmetry[1] = aabb.getL() - aabb.getXmax();
			symmetry[2] = aabb.getYmin();
			symmetry[3] = aabb.getYmax();
			symmetry[4] = aabb.getZmin();
			symmetry[5] = aabb.getZmax();
			break;
		case 5:
			symmetry[0] = aabb.getL() - aabb.getXmin();
			symmetry[1] = aabb.getL() - aabb.getXmax();
			symmetry[2] = aabb.getYmin();
			symmetry[3] = aabb.getYmax();
			symmetry[4] = aabb.getH() - aabb.getZmin();
			symmetry[5] = aabb.getH() - aabb.getZmax();
			break;
		case 6:
			symmetry[0] = aabb.getL() - aabb.getXmin();
			symmetry[1] = aabb.getL() - aabb.getXmax();
			symmetry[2] = aabb.getW() - aabb.getYmin();
			symmetry[3] = aabb.getW() - aabb.getYmax();
			symmetry[4] = aabb.getZmin();
			symmetry[5] = aabb.getZmax();
			break;
		case 7:
			symmetry[0] = aabb.getL() - aabb.getXmin();
			symmetry[1] = aabb.getL() - aabb.getXmax();
			symmetry[2] = aabb.getW() - aabb.getYmin();
			symmetry[3] = aabb.getW() - aabb.getYmax();
			symmetry[4] = aabb.getH() - aabb.getZmin();
			symmetry[5] = aabb.getH() - aabb.getZmax();
			break;
	}

	/*for(int i = 0; i < 6; ++i){
		if(binary[2] == 0){
			binary[2] = 1;
			symmetry[0] = aabb.getXmin();
			symmetry[1] = aabb.getXmax();
		} else {
			symmetry[0] = aabb.getL() - aabb.getXmin();
			symmetry[1] = aabb.getL() - aabb.getXmax();
			if(binary[1] == 0){
				binary[1] = 1;
				binary[2] = 0;
				symmetry[2] = aabb.getYmin();
				symmetry[3] = aabb.getYmax();
			} else {
				symmetry[2] = aabb.getW() - aabb.getYmin();
				symmetry[3] = aabb.getW() - aabb.getYmax();
				if(binary[0] == 0){
					binary[0] = 1;
					binary[1] = 0;
					symmetry[4] = aabb.getZmin();
					symmetry[5] = aabb.getZmax();
				} else {
					symmetry[4] = aabb.getH() - aabb.getZmin();
					symmetry[5] = aabb.getH() - aabb.getZmax();
				}
			}
		}
		symmetries.push_back(symmetry);
	}*/

}

double clpState::diff(const State& s) const{
	const clpState& s2=dynamic_cast<const clpState &>(s);

	//Obtener suma de volumenes de los bloques intersectados
	long interVolume = 0;
	const AABB* aabb = &cont->blocks->top();
	list<const AABB*> inter;
	long symmetry[6];
	long tempVolume;

	//cout << "Volumen s1: " << s1.cont->getVolume() << endl;
	//cout << "Volumen s2: " << s2.cont->getVolume() << endl;
	//cout << "Volumen total: " << s1.cont->getVolume() + s2.cont->getVolume() << endl;
	//Suma de volumenes de ambos contenedores
	do{
		//se obtiene simetria por contenedor (son m·ximo 8 simetrias)
		for(int symmetryIndex = 0; symmetryIndex < 8; symmetryIndex++){
			tempVolume = 0; //volumen temporal
			symmetryVolume(*aabb, symmetry, symmetryIndex); //se obtiene la simetrÌa en funciÛn a un Ìndice

			AABB reflected_aabb = AABB(symmetry[0], symmetry[1], symmetry[2], symmetry[3], symmetry[4], symmetry[5]);
			inter = s2.cont->blocks->get_intersected_objects(reflected_aabb);

			for(const AABB* b:inter)
				tempVolume += place(reflected_aabb.getXmin(), reflected_aabb.getXmax(), b->getXmin(), b->getXmax()) * place(reflected_aabb.getYmin(), reflected_aabb.getYmax(), b->getYmin(), b->getYmax()) * place(reflected_aabb.getZmin(), reflected_aabb.getZmax(), b->getZmin(), b->getZmax());

			if(tempVolume < interVolume)
				interVolume = tempVolume;
		}

		if(cont->blocks->has_next())
			aabb = &cont->blocks->next();
		else break;
	}while(true);

	//Obtener suma de volumenes de los bloques de ambos contenedores
	long int simetricDifVolume = 0;
	//Suma de volumenes del primer contenedor
	aabb = &cont->blocks->top();
	do{
		//cout << "\nVolumenes unidos: " << aabb->getVolume() << endl;
		simetricDifVolume += aabb->getVolume();
		if(cont->blocks->has_next())
			aabb = &cont->blocks->next();
		else break;
	}while(true);

	//Se agregan volumenes del segundo contenedor
	aabb = &s2.cont->blocks->top();
	do{
		//cout << "\nVolumenes unidos: " << aabb->getVolume() << endl;
		simetricDifVolume += aabb->getVolume();
		if(s2.cont->blocks->has_next())
			aabb = &s2.cont->blocks->next();
		else break;
	}while(true);

	//cout << "\nVolumen uniones:\t\t" << simetricDifVolume << endl;

	simetricDifVolume -= interVolume;

	//cout << "Volumen diferencia simetrica:\t" << simetricDifVolume << endl;
	//cout << "Volumen intersecciones:\t\t" << interVolume << endl;
	//cout << "Resta de volumenes:\t\t" << simetricDifVolume - interVolume << endl;


	return (double) (simetricDifVolume - interVolume)/(double) cont->getVolume();

}

void clpState::get_actions(list< Action* >& actions) const{
	list<const Block*>::const_iterator it;

    const Space* sp=NULL;

    //cout << valid_blocks.size() << endl;

	while(cont->spaces->size()>0 && actions.size()==0){


	    sp=&cont->spaces->top();
	    //cout << "spaces:" << cont->spaces->size() << endl;

		for(it = valid_blocks.begin();it!=valid_blocks.end();it++)
			if(**it <= sp->getDimensions()) actions.push_back(new clpAction(**it,*sp));

		//the space is removed
		if(actions.size()==0){
			//cout << "to delete: " << *sp << ";" << sp << endl;
			cont->spaces->pop();
		}
	}
}


void clpState::compute_supports(map<const AABB*,int>& n_supports, set<const AABB*>& zero_support_aabb){
	set<const AABB*> visited;

    //we calculate the number of supported blocks for each block
    const AABB* block = &cont->blocks->top();
    int i=0;
    while(true){
        visited.insert(block);
     //   original_order[block]=i++;

        if(n_supports.find(block)==n_supports.end()) zero_support_aabb.insert(block);

        list<const AABB*> adj = cont->blocks->get_intersected_objects(*block);
        for( auto aabb : adj )
            if(visited.find(aabb)==visited.end()) n_supports[aabb]++;

        if(cont->blocks->has_next()) block = &cont->blocks->next();
        else break;
    }
}

void clpState::update_supports(const AABB* block,
		map<const AABB*,int>& n_supports, set<const AABB*>& zero_support_aabb){

	zero_support_aabb.erase(block);
    list<const AABB*> adj = cont->blocks->get_intersected_objects(*block);
    for( auto aabb : adj )
        if(n_supports.find(aabb)!=n_supports.end()) {
            n_supports[aabb]--;
            if(n_supports[aabb]==0) {
                zero_support_aabb.insert(aabb);
                n_supports.erase(aabb);
            }
        }
}

State* clpState::create_neighbor(State* s0){
	clpState* s1=(clpState*) s0->clone();

	int n=path.size()/2;
	//we recover the partial state between s0 and best_state
	for(int j=0; j<n; j++)
		s1->transition(*s1->next_action(*this));

	list<const Block*> boxes;
	for(auto block:s1->valid_blocks)
		if(block->n_boxes==1) boxes.push_back(block);

	s1->valid_blocks=boxes;

	return s1;
}

bool clpState::left=true;
int clpState::shuffle_path() {

    //number of support blocks for each block
    map<const AABB*,int> n_supports;

    //set of blocks with no supports
    set<const AABB*> zero_support_aabb;

    compute_supports(n_supports, zero_support_aabb);

    list<const Action*> new_path;

    const AABB* block = &cont->blocks->top();
    //se genera el path colocando todos los bloques de la izquierda de cont
    while(true){
    	if(((left && block->getXmin() <= cont->getL() - block->getXmax())||
    			(!left && block->getXmin() > cont->getL() - block->getXmax())) &&
    			zero_support_aabb.find(block)!=zero_support_aabb.end()){


    		 new_path.push_back(new clpAction(*block, *cont));

    		 update_supports(block, n_supports, zero_support_aabb);
    	}

        if(cont->blocks->has_next()) block = &cont->blocks->next();
        else break;
    }

    int i=new_path.size();

    block = &cont->blocks->top();
     //se coloca el resto de loc bloques en el path
     while(true){
     	if(zero_support_aabb.find(block)!=zero_support_aabb.end()){

     		 new_path.push_back(new clpAction(*block, *cont));

     		 update_supports(block, n_supports, zero_support_aabb);
     	}

         if(cont->blocks->has_next()) block = &cont->blocks->next();
         else break;
     }

    while(!path.empty()){
		delete path.front();
		path.pop_front();
	}

    path=new_path;

    left= !left;

    return i;
}

/* TODO: Creacion aleatoria reproducible de pesos para cajas
long rand(long seed)
{
  long a = 1103515245;
  long m = 2<<32;
  long c = 12345;

  seed = (a * seed + c) % m;
  return seed;
}*/

double clpState::weight_of_allboxes=0.0;


clpState* new_state(string file, int i, double min_fr, int max_bl, clpState::Format f){

  clpState::weight_of_allboxes=0.0;

	ifstream in(file.c_str());
	string line;
	if(f==clpState::BR || f==clpState::BRw){
		getline(in,line); //number of instances
	}

	clpState *s=NULL;

	for(int inst=0;inst<=i; inst++){
		string line;

		if(f==clpState::BR || f==clpState::BRw){
			getline(in, line ); //n_inst best_sol?
		}

		getline(in, line); //L W H

		if(inst==i){
			std::stringstream ss(line);
			long l,w,h;
			ss >> l >> w >> h;
			cout << l << " " <<  w << " " << h << endl;
			if(f==clpState::_1C) {l*=10; w*=10; h*=10;}
			s= new clpState((Block::FSB)? new Block_fsb(l,w,h):new Block(l,w,h));
		}

		if(f==clpState::_1C){
			getline(in, line );
			std::stringstream ss1(line);
			ss1 >> clpState::weight_of_allboxes;
		}


		getline(in, line); //types of boxes

		std::stringstream ss0(line);
		int nb_types;
		ss0 >> nb_types;

		//se lee el archivo de entrada
		//Objetos BoxType, guardan los datos para cada tipo de cajas: dimensiones (w x l x h)
		//y restricciones de rotaci√≥n
		//En el objeto clp se agregan los tipos de cajas y el n√∫mero de elementos que hay de cada tipo

		//clpState::weight_of_allboxes=0.0;

		for(int j=0;j<nb_types;j++){
			getline(in, line );


			int n, id;
			long l,h,w;
			double weight = 1.0;
			double vol;
			bool rot1, rot2, rot3;
			std::stringstream ss1(line);

			if(f==clpState::BR){
				ss1 >> id >> l >> rot1 >> w >> rot2 >> h >> rot3 >> n;
				vol=l*h*w;
			}if(f==clpState::BRw){
				ss1 >> id >> l >> rot1 >> w >> rot2 >> h >> rot3 >> n >> weight;
				vol=l*h*w;
			}else if(f==clpState::_1C){
				double ll,hh,ww;
				ss1 >> ll >> rot1 >> ww >> rot2 >> hh >> rot3 >> weight >> n;
				ll*=10; ww*=10; hh*=10;
				l = ceil(ll);
				w = ceil(ww);
				h = ceil(hh);
				vol=ll*hh*ww;
			}



			if(inst==i){

				BoxShape* boxt=new BoxShape(id, l, w, h, rot1, rot2, rot3, weight);

				if (f==clpState::BR || f==clpState::BRw) clpState::weight_of_allboxes += weight*(double) n;

				s->nb_left_boxes.insert(make_pair(boxt,n));
				for(int o=0; o<6; o++){
					if(boxt->is_valid((BoxShape::Orientation) o)){
						if(!Block::FSB)
							s->valid_blocks.push_back(new Block(*boxt,(BoxShape::Orientation) o, vol));
						else
							s->valid_blocks.push_back(new Block_fsb(*boxt,(BoxShape::Orientation) o, vol));
					}
				}
				//cout << "Wmax:" << clpState::weight_of_allboxes << endl;
			}
		}

	}


	s->general_block_generator(min_fr, max_bl, *s->cont);

	s->update_min_dim();

	return s;
}

void clpState::general_block_generator(double min_fr, int max_bl, const Vector3& max_dim){

    list<const Block*>& B=valid_blocks;
    list<const Block*> P=B;
	while(B.size()<max_bl){
		list<const Block*>  N;
		list<const Block*> :: iterator itP=P.begin();
	    int new_elems=0;
	    for(;itP!=P.end() && B.size()+new_elems<max_bl; itP++){
	    	list<const Block*> :: iterator itB=B.begin();
	    	for(;itB!=B.end() && B.size()+new_elems<max_bl ; itB++){

	    		//cout << "new_blocks" << endl;
				list<const Block*> newB = (*itP)->create_new_blocks(*itB, min_fr, max_dim);


				list<const Block*>:: iterator itNew=newB.begin();
				for(;itNew!=newB.end();itNew++){
				   if(is_constructible(*this,**itNew) && **itNew <= *cont){
					  int NoldSize=N.size();
					  N.push_back(*itNew);
					  new_elems++;

					  if(B.size()+new_elems>=max_bl) break;
				   }else { /*delete *itNew;*/ }
				}
	    	}
	    }
		if(N.size()==0) break;
		B.insert(B.end(),N.begin(),N.end());
		P=N;
	}
}



void clpState::update_min_dim(){
	map<const BoxShape*, int>::const_iterator it= nb_left_boxes.begin();
	for(;it!=nb_left_boxes.end();it++){
		if(it->second >0){
			for(int o=0; o<6; o++)
				if(it->first->is_valid((BoxShape::Orientation) o)){
					if(mindim.getX() > it->first->getL((BoxShape::Orientation) o) )
						mindim.setX(it->first->getL((BoxShape::Orientation) o));

					if(mindim.getY() > it->first->getW((BoxShape::Orientation) o) )
						mindim.setY(it->first->getW((BoxShape::Orientation) o));

					if(mindim.getZ() > it->first->getH((BoxShape::Orientation) o) )
						mindim.setZ(it->first->getH((BoxShape::Orientation) o));
				}
		}
	}
}



void clpState::_transition(const Action& action) {
	const clpAction& act = *dynamic_cast<const clpAction*> (&action);

	const Block& b= act.block;

	//se actualizan las cajas restantes
	bool update=false;
	map<const BoxShape*, int>::const_iterator it= b.nb_boxes.begin();
	for(;it!=b.nb_boxes.end();it++){
		nb_left_boxes[it->first] -= it->second;
		if(nb_left_boxes[it->first]==0) update=true;
	}
	//if(update) update_min_dim();

	//se inserta el bloque en el contenedor
	cont->insert(b, act.space.get_location(b), mindim);

	//se actualizan los bloques validos
	update_valid_blocks();
}

bool is_constructible(const clpState& s, const Block& b){
	map<const BoxShape*, int>::const_iterator it= b.nb_boxes.begin();
	for(;it!=b.nb_boxes.end();it++)
		if(s.nb_left_boxes.at(it->first) < it->second)
			return false;

	return true;
}


}
