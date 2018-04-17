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

#include "objects2/BoxShape.h"
#include "clpState.h"

using namespace std;
using namespace metasolver;

namespace clp {


void clpState::get_actions(list< Action* >& actions) const{
	list<const Block*>::const_iterator it;

    const Space* sp=NULL;

   // cout << valid_blocks.size() << endl;

	while(cont->spaces->size()>0 && actions.size()==0){
		//cout << "spaces:" << cont.spaces->size() << endl;

	    sp=&cont->spaces->top();

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
		//y restricciones de rotación
		//En el objeto clp se agregan los tipos de cajas y el número de elementos que hay de cada tipo

		//clpState::weight_of_allboxes=0.0;

		for(int j=0;j<nb_types;j++){
			getline(in, line );


			int n, id;
			long l,h,w;
			double weight = 1.0;
			bool rot1, rot2, rot3;
			std::stringstream ss1(line);

			if(f==clpState::BR)
				ss1 >> id >> l >> rot1 >> w >> rot2 >> h >> rot3 >> n;

			if(f==clpState::BRw)
				ss1 >> id >> l >> rot1 >> w >> rot2 >> h >> rot3 >> n >> weight;

			else if(f==clpState::_1C){
				double ll,hh,ww;
				ss1 >> ll >> rot1 >> ww >> rot2 >> hh >> rot3 >> weight >> n;
				l = ceil(ll);
				w = ceil(ww);
				h = ceil(hh);
			}



			if(inst==i){

				BoxShape* boxt=new BoxShape(id, l, w, h, rot1, rot2, rot3, weight);

				//cout << weight << " x " << n <<  endl;
				if (f==clpState::BR || f==clpState::BRw) clpState::weight_of_allboxes += weight*(double) n;

				s->nb_left_boxes.insert(make_pair(boxt,n));
				for(int o=0; o<6; o++){
					if(boxt->is_valid((BoxShape::Orientation) o)){
						if(!Block::FSB)
							s->valid_blocks.push_back(new Block(*boxt,(BoxShape::Orientation) o));
						else
							s->valid_blocks.push_back(new Block_fsb(*boxt,(BoxShape::Orientation) o));
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
