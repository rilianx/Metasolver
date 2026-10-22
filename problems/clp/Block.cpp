/*
 * Block.cpp
 *
 *  Created on: 01-06-2017
 *      Author: ignacio
 */

#include <algorithm>
#include <unordered_set>
#include <unordered_map>
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
	double weight;
	//la cara superior esta completamente cubierta por cajas
	bool top_full;
	//contiene alguna caja fragil
	bool fragile;
	//se consideran top_full y fragile al comparar bloques
	bool support;
	//centro de gravedad relativo a la esquina
	double cog[3];
	//c1 en la esquina y c2 desplazado en off (NULL si es de una caja, de tipo type)
	const GenBlock *c1, *c2;
	long off[3];
	int type;

	bool supportive() const { return top_full && !fragile; }
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
			   b1->occupied_volume==b2->occupied_volume && b1->boxes==b2->boxes &&
			   (!b1->support || b1->top_full==b2->top_full);
	}
};

/**
 * une las cajas de b1 y b2 en un nuevo bloque de dimensiones l,w,h: b1 en la esquina y b2
 * a continuacion en la direccion dir (0:x, 1:y, 2:z)
 */
GenBlock* join(const GenBlock* b1, const GenBlock* b2, long l, long w, long h, int dir){
	GenBlock* b = new GenBlock();
	b->l=l; b->w=w; b->h=h;
	b->c1=b1; b->c2=b2; b->type=-1;
	b->off[0] = (dir==0)? b1->l : 0;
	b->off[1] = (dir==1)? b1->w : 0;
	b->off[2] = (dir==2)? b1->h : 0;
	double wt = b1->weight + b2->weight;
	for(int d=0; d<3; d++)
		b->cog[d] = (wt>0)? (b1->weight*b1->cog[d] + b2->weight*(b2->cog[d]+b->off[d]))/wt
				: (d==0? l : d==1? w : h)/2.0;
	b->occupied_volume = b1->occupied_volume + b2->occupied_volume;
	b->n_boxes = b1->n_boxes + b2->n_boxes;
	b->weight = b1->weight + b2->weight;
	b->fragile = b1->fragile || b2->fragile;
	b->support = b1->support;
	b->boxes.reserve(b1->boxes.size() + b2->boxes.size());
	auto i=b1->boxes.begin(), j=b2->boxes.begin();
	while(i!=b1->boxes.end() || j!=b2->boxes.end()){
		if(j==b2->boxes.end() || (i!=b1->boxes.end() && i->first < j->first)) b->boxes.push_back(*i++);
		else if(i==b1->boxes.end() || j->first < i->first) b->boxes.push_back(*j++);
		else { b->boxes.push_back(make_pair(i->first, i->second + j->second)); i++; j++; }
	}
	return b;
}

/** las cajas de b1 y b2 juntas estan disponibles */
bool is_constructible(const vector<int>& left, const GenBlock& b1, const GenBlock& b2){
	auto i=b1.boxes.begin(), j=b2.boxes.begin();
	while(i!=b1.boxes.end() || j!=b2.boxes.end()){
		if(j==b2.boxes.end() || (i!=b1.boxes.end() && i->first < j->first)){
			if(left[i->first] < i->second) return false;
			i++;
		}else if(i==b1.boxes.end() || j->first < i->first){
			if(left[j->first] < j->second) return false;
			j++;
		}else{
			if(left[i->first] < i->second + j->second) return false;
			i++; j++;
		}
	}
	return true;
}

}

vector<Block> Block::all;
vector<Block::BoxCount> Block::all_boxes;
vector<Block::PlacedBox> Block::all_pboxes;

vector<const Block*> Block::generate_blocks(const vector<const BoxShape*>& types,
		const vector<int>& left, double min_fr, int max_bl, long L, long W, long H, bool support){

	//bloques de una caja
	vector<const GenBlock*> B;
	for(const BoxShape* box : types)
		for(int o=0; o<6; o++)
			if(box->is_valid(o)){
				GenBlock* b = new GenBlock();
				b->l=box->getL(o); b->w=box->getW(o); b->h=box->getH(o);
				b->occupied_volume=box->getVolume();
				b->n_boxes=1;
				b->weight=box->weight;
				b->top_full=true;
				b->fragile=box->fragile;
				b->support=support;
				b->c1=b->c2=NULL; b->type=box->type;
				b->cog[0]=b->l/2.0; b->cog[1]=b->w/2.0; b->cog[2]=b->h/2.0;
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
				//las combinaciones que no se pueden construir con las cajas disponibles se descartan
				//(se revisa solo si alguna direccion cumple las dimensiones y el volumen minimo)
				bool checked=false;

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

					if(!checked){
						if(!is_constructible(left, *b1, *b2)) break;
						checked=true;
					}

					//cara superior del bloque resultante
					bool top_full=false;
					const GenBlock *first=b1, *second=b2;
					if(support){
						if(i==2){
							//el bloque de abajo debe soportar completamente al de arriba
							const GenBlock *down=b1, *up=b2;
							if(!(down->supportive() && up->l<=down->l && up->w<=down->w)) swap(down, up);
							if(!(down->supportive() && up->l<=down->l && up->w<=down->w)) continue;
							top_full = up->top_full && up->l==down->l && up->w==down->w;
							first=down; second=up;
						}else
							top_full = b1->top_full && b2->top_full && b1->h==b2->h &&
								((i==0)? b1->w==b2->w : b1->l==b2->l);
					}

					GenBlock* nb = join(first, second, ll, ww, hh, i);
					nb->top_full = top_full;
					if(!all_blocks.insert(nb).second) { delete nb; continue; }

					N.push_back(nb);
					if(nB+(int)N.size()>=max_bl) break;
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
	unordered_map<const GenBlock*, int> index;
	for(size_t k=0; k<B.size(); k++) index[B[k]]=k;
	for(const GenBlock* g : B){
		Block b;
		b.id = all.size();
		b.l=g->l; b.w=g->w; b.h=g->h;
		b.occupied_volume=g->occupied_volume;
		b.n_boxes=g->n_boxes;
		b.weight=g->weight;
		b.supportive=g->supportive();
		b.fragile=g->fragile;
		for(int d=0; d<3; d++){ b.cog[d]=g->cog[d]; b.offset[d]= g->c1? g->off[d] : 0; }
		b.child[0] = g->c1? index[g->c1] : -1;
		b.child[1] = g->c2? index[g->c2] : -1;
		b.box_type = g->type;
		size_t first = all_boxes.size();
		all_boxes.insert(all_boxes.end(), g->boxes.begin(), g->boxes.end());
		b.boxes.first = all_boxes.data()+first;
		b.boxes.last = all_boxes.data()+all_boxes.size();
		all.push_back(b);
	}
	for(const GenBlock* g : all_blocks) delete g;
	for(size_t k=0; k<B.size(); k++) if(B[k]->n_boxes==1) delete B[k];

	all_pboxes.clear();
	for(Block& b : all) b.pboxes.first = b.pboxes.last = 0;

	vector<const Block*> blocks;
	for(const Block& b : all) blocks.push_back(&b);
	return blocks;
}

void Block::compute_placed_boxes(){
	all_pboxes.clear();
	for(Block& b : all){
		b.pboxes.first = all_pboxes.size();
		b.get_boxes(0, 0, 0, all_pboxes);
		b.pboxes.last = all_pboxes.size();
	}
}

void Block::get_boxes(long x, long y, long z, vector<PlacedBox>& boxes) const{
	if(child[0]<0){
		boxes.push_back(PlacedBox{box_type, x, y, z, x+l, y+w, z+h});
		return;
	}
	all[child[0]].get_boxes(x, y, z, boxes);
	all[child[1]].get_boxes(x+offset[0], y+offset[1], z+offset[2], boxes);
}

} /* namespace clp */
