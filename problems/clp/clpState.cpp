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


clpState* new_state(string file, int i, double min_fr, int max_bl){

	ifstream in(file.c_str());
	string line;
	getline(in,line); //number of instances

	clpState *s=NULL;

	for(int inst=0;inst<=i; inst++){
		string line;

		getline(in, line ); //n_inst random_seed
		getline(in, line); //L W H

		if(inst==i){
			std::stringstream ss(line);
			long l,w,h;
			ss >> l >> w >> h;
			cout << l << " " <<  w << " " << h << endl;
			s= new clpState(new Block(l,w,h));
		}

		getline(in, line); //types of boxes

		std::stringstream ss0(line);
		int nb_types;
		ss0 >> nb_types;

		//se lee el archivo de entrada
		//Objetos BoxShape, guardan los datos para cada tipo de cajas: dimensiones (w x l x h)
		//y restricciones de rotación
		//En el objeto clp se agregan los tipos de cajas y el número de elementos que hay de cada tipo
		for(int j=0;j<nb_types;j++){
			getline(in, line );

			int n, id;
			long l,h,w;
			bool rot1, rot2, rot3;
			std::stringstream ss1(line);
			ss1 >> id >> l >> rot1 >> w >> rot2 >> h >> rot3 >> n;
			double vol=l*h*w;

			if(inst==i){
				BoxShape* boxt=new BoxShape(id, l, w, h, rot1, rot2, rot3);

				s->nb_left_boxes.insert(make_pair(boxt,n));
				for(int o=0; o<6; o++)
					if(boxt->is_valid((BoxShape::Orientation) o))
						s->valid_blocks.push_back(new Block(*boxt,(BoxShape::Orientation) o, vol));
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
