/*
 * Block.cpp
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

#include <algorithm>
#include <unordered_set>
#include <cstdint>
#include "Block.h"

using namespace std;

namespace clp {

namespace {

/** bloque durante la generacion */
struct GenBlock {
	long l, w, h;
	long occupied_volume;
	int n_boxes;
	vector<Block::BoxCount> boxes;
};

// hash de bloques (ver Apendice A de la disertacion de L. Guesser)
inline uint64_t hash_mix(uint64_t h){
	h ^= h >> 32; h *= 0xe9846af9b1a615dull;
	h ^= h >> 32; h *= 0xe9846af9b1a615dull;
	h ^= h >> 28;
	return h;
}

inline uint64_t hash_combine(uint64_t l, uint64_t r){
	return hash_mix((l ^ 0x9e3779b9) - r);
}

struct BlockHash {
	size_t operator()(const GenBlock* b) const {
		uint64_t h = hash_mix(b->l);
		h = hash_combine(h, hash_mix(b->w));
		h = hash_combine(h, hash_mix(b->h));
		for(auto& p : b->boxes){
			h = hash_combine(h, hash_mix(p.first));
			h = hash_combine(h, hash_mix(p.second));
		}
		return h;
	}
};

struct BlockEqual {
	bool operator()(const GenBlock* b1, const GenBlock* b2) const {
		return b1->l==b2->l && b1->w==b2->w && b1->h==b2->h &&
			   b1->occupied_volume==b2->occupied_volume && b1->boxes==b2->boxes;
	}
};

/** une las cajas de b1 y b2 en un nuevo bloque de dimensiones l,w,h */
GenBlock* join(const GenBlock* b1, const GenBlock* b2, long l, long w, long h){
	GenBlock* b = new GenBlock();
	b->l=l; b->w=w; b->h=h;
	b->occupied_volume = b1->occupied_volume + b2->occupied_volume;
	b->n_boxes = b1->n_boxes + b2->n_boxes;
	b->boxes.reserve(b1->boxes.size() + b2->boxes.size());
	auto i=b1->boxes.begin(), j=b2->boxes.begin();
	while(i!=b1->boxes.end() || j!=b2->boxes.end()){
		if(j==b2->boxes.end() || (i!=b1->boxes.end() && i->first < j->first)) b->boxes.push_back(*i++);
		else if(i==b1->boxes.end() || j->first < i->first) b->boxes.push_back(*j++);
		else { b->boxes.push_back(make_pair(i->first, i->second + j->second)); i++; j++; }
	}
	return b;
}

bool is_constructible(const vector<int>& left, const GenBlock& b){
	for(auto& p : b.boxes)
		if(left[p.first] < p.second) return false;
	return true;
}

}

vector<Block> Block::all;
vector<Block::BoxCount> Block::all_boxes;

vector<const Block*> Block::generate_blocks(const vector<const BoxShape*>& types,
		const vector<int>& left, double min_fr, int max_bl, long L, long W, long H){

	//bloques de una caja
	vector<const GenBlock*> B;
	for(const BoxShape* box : types)
		for(int o=0; o<6; o++)
			if(box->is_valid(o)){
				GenBlock* b = new GenBlock();
				b->l=box->getL(o); b->w=box->getW(o); b->h=box->getH(o);
				b->occupied_volume=box->getVolume();
				b->n_boxes=1;
				b->boxes.push_back(make_pair(box->type, 1));
				B.push_back(b);
			}

	unordered_set<const GenBlock*, BlockHash, BlockEqual> all_blocks;
	vector<const GenBlock*> P=B;

	while((int)B.size()<max_bl){
		vector<const GenBlock*> N;
		int nB = B.size();
		for(size_t ip=0; ip<P.size() && nB+(int)N.size()<max_bl; ip++){
			const GenBlock* b1 = P[ip];
			for(int ib=0; ib<nB && nB+(int)N.size()<max_bl; ib++){
				const GenBlock* b2 = B[ib];

				for(int i=0; i<3; i++){
					long ll= max(b1->l, b2->l);
					long ww= max(b1->w, b2->w);
					long hh= max(b1->h, b2->h);
					switch(i){
						case 0: ll=b1->l+b2->l; break;
						case 1: ww=b1->w+b2->w; break;
						case 2: hh=b1->h+b2->h; break;
					}

					double vol = (double) ll*ww*hh;
					if((double)(b1->occupied_volume+b2->occupied_volume) / vol < min_fr ||
							ll>L || ww>W || hh>H) continue;

					GenBlock* nb = join(b1, b2, ll, ww, hh);
					if(!all_blocks.insert(nb).second) { delete nb; continue; }

					if(is_constructible(left, *nb)){
						N.push_back(nb);
						if(nB+(int)N.size()>=max_bl) break;
					}
				}
			}
		}
		if(N.empty()) break;
		B.insert(B.end(), N.begin(), N.end());
		P=N;
	}

	//se copian los bloques a los arreglos contiguos
	size_t nboxes=0;
	for(const GenBlock* g : B) nboxes += g->boxes.size();
	all_boxes.clear(); all_boxes.reserve(nboxes);
	all.clear(); all.reserve(B.size());
	for(const GenBlock* g : B){
		Block b;
		b.id = all.size();
		b.l=g->l; b.w=g->w; b.h=g->h;
		b.occupied_volume=g->occupied_volume;
		b.n_boxes=g->n_boxes;
		size_t first = all_boxes.size();
		all_boxes.insert(all_boxes.end(), g->boxes.begin(), g->boxes.end());
		b.boxes.first = all_boxes.data()+first;
		b.boxes.last = all_boxes.data()+all_boxes.size();
		all.push_back(b);
	}
	//(los bloques de una caja no estan en all_blocks; se borran antes que los otros, que se
	//leen al revisar n_boxes)
	for(size_t k=0; k<B.size(); k++) if(B[k]->n_boxes==1) delete B[k];
	for(const GenBlock* g : all_blocks) delete g;

	vector<const Block*> blocks;
	for(const Block& b : all) blocks.push_back(&b);
	return blocks;
}

} /* namespace clp */
