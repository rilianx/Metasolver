/*
 * clpState.cpp
 *
 *  Created on: 4 jul. 2017
 *      Author: iaraya
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "clpState.h"
#include "VCS_Function.h"

using namespace std;
using namespace metasolver;

namespace clp {

vector<const BoxShape*> clpState::types;

static inline uint64_t mix(uint64_t h){
	h ^= h >> 33; h *= 0xff51afd7ed558ccdull;
	h ^= h >> 33; h *= 0xc4ceb9fe1a85ec53ull;
	h ^= h >> 33;
	return h;
}

static inline uint64_t placement_hash(int id, long x, long y, long z){
	return mix(mix(mix(mix((uint64_t) id) ^ (uint64_t) x) ^ (uint64_t) y) ^ (uint64_t) z);
}

int clpState::get_best_actions(ActionEvaluator* evl, list< Action* >& bactions, int n) const{
	VCS_Function& vcs = *static_cast<VCS_Function*>(evl);

	while(cont.spaces.size()>0){
		int isp = cont.top_space();
		const Space& sp = cont.spaces[isp];

		vcs.set_space(*this, sp);

		//las mejores n acciones ordenadas de peor a mejor (en caso de empate,
		//la evaluada primero se considera peor)
		static vector< pair<double, const Block*> > ranked;
		ranked.clear();
		bool any=false;
		for(const Block* b : valid_blocks){
			if(!b->fits(sp.getL(), sp.getW(), sp.getH())) continue;
			any=true;
			double eval = vcs.eval(*this, *b, sp);
			if(eval>0 && ((int)ranked.size()<n || ranked.front().first < eval)){
				auto pos = upper_bound(ranked.begin(), ranked.end(), eval,
						[](double e, const pair<double, const Block*>& r){ return e < r.first; });
				ranked.insert(pos, make_pair(eval, b));
				if((int)ranked.size()==n+1) ranked.erase(ranked.begin());
			}
		}

		//el espacio se elimina si no cabe ningun bloque
		if(!any) { cont.remove_space(isp); continue; }

		for(auto it=ranked.rbegin(); it!=ranked.rend(); it++)
			bactions.push_back(new clpAction(*it->second, sp));

		return bactions.size();
	}
	return 0;
}

void clpState::_transition(const Action& action) {
	const clpAction& act = static_cast<const clpAction&>(action);
	const Block& b= act.block;

	//se actualizan las cajas restantes
	for(auto& p : b.boxes) left[p.first] -= p.second;

	//se inserta el bloque en el contenedor
	long x, y, z;
	act.space.get_location(b.getL(), b.getW(), b.getH(), x, y, z);
	cont.insert(b, x, y, z, min_l, min_w, min_h);

	hash += placement_hash(b.id, x, y, z);

	update_valid_blocks();
}

void clpState::update_valid_blocks() {
	valid_blocks.erase(remove_if(valid_blocks.begin(), valid_blocks.end(),
		[this](const Block* b){
			for(auto& p : b->boxes)
				if(left[p.first] < p.second) return true;
			return false;
		}), valid_blocks.end());
}

void clpState::update_min_dim(){
	for(const BoxShape* box : types){
		if(left[box->type] <= 0) continue;
		for(int o=0; o<6; o++)
			if(box->is_valid(o)){
				min_l = min(min_l, box->getL(o));
				min_w = min(min_w, box->getW(o));
				min_h = min(min_h, box->getH(o));
			}
	}
}

clpState* new_state(string file, int i, double min_fr, int max_bl){

	ifstream in(file.c_str());
	if(!in){
		cerr << "No se pudo abrir el archivo " << file << endl;
		exit(1);
	}

	string line;
	getline(in,line); //number of instances

	clpState *s=NULL;

	for(int inst=0;inst<=i; inst++){
		getline(in, line); //n_inst random_seed
		getline(in, line); //L W H

		if(inst==i){
			std::stringstream ss(line);
			long l,w,h;
			ss >> l >> w >> h;
			cout << l << " " <<  w << " " << h << endl;
			s= new clpState(l, w, h);
		}

		getline(in, line); //types of boxes
		std::stringstream ss0(line);
		int nb_types;
		ss0 >> nb_types;

		//cada linea: id l rot1 w rot2 h rot3 n
		for(int j=0;j<nb_types;j++){
			getline(in, line);
			if(inst!=i) continue;

			int n, id;
			long l,h,w;
			bool rot1, rot2, rot3;
			std::stringstream ss1(line);
			ss1 >> id >> l >> rot1 >> w >> rot2 >> h >> rot3 >> n;

			BoxShape* box=new BoxShape(j, id, l, w, h, rot1, rot2, rot3);
			clpState::types.push_back(box);
			s->left.push_back(n);

		}
	}

	s->valid_blocks = Block::generate_blocks(clpState::types, s->left, min_fr, max_bl,
			s->cont.getL(), s->cont.getW(), s->cont.getH());
	s->update_min_dim();

	return s;
}

} /* namespace clp */
