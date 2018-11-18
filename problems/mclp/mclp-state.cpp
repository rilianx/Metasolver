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
#include "mclp-state.h"

using namespace std;
using namespace metasolver;

namespace clp {

//TODO: ¿Que hacemos aqui?
void mclpState::get_actions(list< Action* >& actions) const{
	/*
	list<const Block*>::const_iterator it;
    const Space* sp=NULL;
	while(cont->spaces->size()>0 && actions.size()==0){
	    sp=&cont->spaces->top();
	    //cout << "spaces:" << cont->spaces->size() << endl;
		for(it = valid_blocks.begin();it!=valid_blocks.end();it++)
			if(**it <= sp->getDimensions()) actions.push_back(new mclpAction(**it,*sp));
		//the space is removed
		if(actions.size()==0){
			cont->spaces->pop();
		}
	}
	*/
}

mclpState* new_mstate(string file, int i, double min_fr, int max_bl){

	mclpState *s = NULL;
	//TODO: Aqui se debería leer el archivo y almacenar en el estado

	ifstream in(file.c_str());
	string line;

	/*if(f==mclpState::BR || f==mclpState::BRw){
		getline(in,line); //number of instances
	}*/
	for(int inst = 0; inst <= i; inst++){
		getline(in,line);
		string line;
		int nb_types;
		/*if(f==mclpState::BR || f==mclpState::BRw){
			getline(in, line ); //n_inst best_sol?
		}*/
		getline(in, line); //nb_types L W H
		std::stringstream ss(line);
		ss >> nb_types;
		if(inst == i){
			long l, w, h;
			ss >> w >> h >> l;
			cout << l << " " <<  w << " " << h << endl;
			s = new mclpState((Block::FSB)? new Block_fsb(l,w,h):new Block(l,w,h));
		}
		//se lee el archivo de entrada
		//Objetos BoxType, guardan los datos para cada tipo de cajas: dimensiones (w x l x h)
		//y restricciones de rotación
		//En el objeto clp se agregan los tipos de cajas y el número de elementos que hay de cada tipo
		//clpState::weight_of_allboxes=0.0;
		for(int j = 0; j < nb_types; j++){
			getline(in, line);
			long l, h, w;
			double weight = 1.0;
			double vol;
			bool rot1 = 1, rot2 = 1, rot3 = 1;
			std::stringstream ss1(line);
			ss1 >> w >> h >> l;
			vol = l * h * w;

			if(inst==i){
				//TODO: esto de aqui genera los bloques unitario (con una caja)
				BoxShape* boxt = new BoxShape(j, l, w, h, rot1, rot2, rot3, weight);
				s->nb_left_boxes.insert(make_pair(boxt, nb_types));
				for(int o = 0; o < 6; o++){
					if(boxt->is_valid((BoxShape::Orientation) o)){
						if(!Block::FSB)
							s->valid_blocks.push_back(new Block(*boxt,(BoxShape::Orientation) o, vol));
						else
							s->valid_blocks.push_back(new Block_fsb(*boxt,(BoxShape::Orientation) o, vol));
					}
				}
			}
		}
	}

	Vector3 maxdim(0,0,0);
	for(auto p:s->pallets){
		if(p->getL() > maxdim.getX()) maxdim.setX(p->getL());
		if(p->getW() > maxdim.getY()) maxdim.setY(p->getW());
		if(p->getH() > maxdim.getZ()) maxdim.setZ(p->getH());
	}

	s->general_block_generator(min_fr, max_bl, maxdim);

	return s;
}

void mclpState::general_block_generator(double min_fr, int max_bl, const Vector3& max_dim){

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
				   if(is_constructible(*this,**itNew) && **itNew <= max_dim){
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

void mclpState::_transition(const Action& action) {
	const mclpAction& act = *dynamic_cast<const mclpAction*> (&action);

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

	pallets[act.id_pallet]->insert(b, act.space.get_location(b), Vector3(0,0,0));

	//se actualizan los bloques validos
	update_valid_blocks();
}

bool is_constructible(const mclpState& s, const Block& b){
	map<const BoxShape*, int>::const_iterator it= b.nb_boxes.begin();
	for(;it!=b.nb_boxes.end();it++)
		if(s.nb_left_boxes.at(it->first) < it->second)
			return false;

	return true;
}


}
