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
	size_t operator()(const Block* b) const {
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
	bool operator()(const Block* b1, const Block* b2) const {
		return b1->l==b2->l && b1->w==b2->w && b1->h==b2->h &&
			   b1->occupied_volume==b2->occupied_volume && b1->boxes==b2->boxes;
	}
};

/** une las cajas de b1 y b2 en un nuevo bloque de dimensiones l,w,h */
Block* join(const Block* b1, const Block* b2, long l, long w, long h){
	Block* b = new Block(l, w, h);
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

bool is_constructible(const vector<int>& left, const Block& b){
	for(auto& p : b.boxes)
		if(left[p.first] < p.second) return false;
	return true;
}

}

void Block::generate_blocks(vector<const Block*>& B, const vector<int>& left,
		double min_fr, int max_bl, long L, long W, long H){

	unordered_set<const Block*, BlockHash, BlockEqual> all_blocks;
	vector<const Block*> P=B;

	while((int)B.size()<max_bl){
		vector<const Block*> N;
		int nB = B.size();
		for(size_t ip=0; ip<P.size() && nB+(int)N.size()<max_bl; ip++){
			const Block* b1 = P[ip];
			for(int ib=0; ib<nB && nB+(int)N.size()<max_bl; ib++){
				const Block* b2 = B[ib];

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

					Block* nb = join(b1, b2, ll, ww, hh);
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
}

} /* namespace clp */
