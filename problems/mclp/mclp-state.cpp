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

map<const BoxShape*, double> mclpState::priority_boxes;

mclpState* new_mstate(string file, int i, double min_fr, int max_bl, bool rot, int nboxes){

	mclpState *s = NULL;
	//TODO: Aqui se deber���������a leer el archivo y almacenar en el estado

	ifstream in(file.c_str());
	string line;


	for(int inst = 0; inst <= i; inst++){
		getline(in,line);
		string line;
		int nb_types;

		getline(in, line); //nb_types L W H
		std::stringstream ss(line);
		ss >> nb_types;
		if(inst == i){
			long l, w, h;
			ss >> w >> h >> l;
			cout << "cont:" << l << " " <<  w << " " << h << endl;
			s = new mclpState(new Block(l,w,h));

			//s = new mclpState((Block::FSB)? new Block_fsb(l,w,h):new Block(l,w,h));
		}
		//se lee el archivo de entrada
		//Objetos BoxType, guardan los datos para cada tipo de cajas: dimensiones (w x l x h)
		//y restricciones de rotaci���������n
		//En el objeto clp se agregan los tipos de cajas y el n���������mero de elementos que hay de cada tipo
		//clpState::weight_of_allboxes=0.0;
		for(int j = 0; j < nb_types; j++){
			getline(in, line);
			long l, h, w;

			bool rot1 = 0, rot2 = 0, rot3 = 0;
			double vol;
			if(rot) {rot1 = 1; rot2 = 1; rot3 = 1;}

			std::stringstream ss1(line);
			ss1 >> w >> h >> l;

			vol = l * h * w;
			double weight = 1;

			if(inst==i){
				//cout << l << " " << h << " " << w << endl;
				//TODO: esto de aqui genera los bloques unitario (con una caja)
				BoxShape* boxt = new BoxShape(j, l, w, h, rot1, rot2, rot3, weight);
				boxt->set_priority(1.0);
				clpState::weight_of_allboxes += weight*(double) nboxes;

				s->nb_left_boxes.insert(make_pair(boxt, nboxes));


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

	s->general_block_generator(min_fr, max_bl, *s->cont);

	s->update_min_dim();

	return s;
}


}
