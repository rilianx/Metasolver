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
#include <cmath>

#include "clpState.h"
#include "VCS_Function.h"

using namespace std;
using namespace metasolver;

namespace clp {

vector<const BoxShape*> clpState::types;
vector<int> clpState::initial_boxes;
const ULD* clpState::uld=NULL;
bool clpState::support=false;
bool clpState::corner_support=false;

//caras superiores de las cajas en la base del espacio actual (criterio de las esquinas)
struct Face { long x1, y1, x2, y2; bool fragile; };
static vector<Face> support_faces;

/** las cuatro esquinas de la base de cada caja inferior del bloque b ubicado en (x,y) se
 * apoyan en caras de cajas no fragiles, y ninguna se apoya sobre una caja fragil */
static bool corners_supported(const Block& b, long x, long y, const vector<Face>& faces){
	for(const Block::PlacedBox& p : b.pboxes){
		if(p.zmin>0) continue;
		long x1=x+p.xmin, y1=y+p.ymin, x2=x+p.xmax, y2=y+p.ymax;
		int corners=0;
		for(const Face& f : faces){
			if(f.fragile){
				if(min(x2, f.x2)>max(x1, f.x1) && min(y2, f.y2)>max(y1, f.y1)) return false;
				continue;
			}
			if(!(corners&1) && f.x1<=x1 && x1<=f.x2 && f.y1<=y1 && y1<=f.y2) corners|=1;
			if(!(corners&2) && f.x1<=x2 && x2<=f.x2 && f.y1<=y1 && y1<=f.y2) corners|=2;
			if(!(corners&4) && f.x1<=x1 && x1<=f.x2 && f.y1<=y2 && y2<=f.y2) corners|=4;
			if(!(corners&8) && f.x1<=x2 && x2<=f.x2 && f.y1<=y2 && y2<=f.y2) corners|=8;
		}
		if(corners!=15) return false;
	}
	return true;
}
double clpState::cog_penalty=0.0;

//bloques ubicados que pueden soportar a otros en la base del espacio actual
static vector<const PlacedBlock*> support_tops;

/** area de [x,x+l) x [y,y+w) cubierta por las caras superiores de los bloques tops */
static long covered_area(const vector<const PlacedBlock*>& tops, long x, long y, long l, long w){
	long area=0;
	for(const PlacedBlock* t : tops){
		long dx = min(x+l, t->xmax) - max(x, t->xmin);
		long dy = min(y+w, t->ymax) - max(y, t->ymin);
		if(dx>0 && dy>0) area += dx*dy;
	}
	return area;
}

static inline uint64_t mix(uint64_t h){
	h ^= h >> 33; h *= 0xff51afd7ed558ccdull;
	h ^= h >> 33; h *= 0xc4ceb9fe1a85ec53ull;
	h ^= h >> 33;
	return h;
}

//(mix(0)=0: sin la constante, el bloque 0 en el origen tendria hash 0 y los estados
//con y sin el serian equivalentes para el greedy)
static inline uint64_t placement_hash(int id, long x, long y, long z){
	return mix(mix(mix(mix((uint64_t) id + 0x9e3779b97f4a7c15ull) ^ (uint64_t) x) ^ (uint64_t) y) ^ (uint64_t) z);
}

int clpState::get_best_actions(ActionEvaluator* evl, list< Action* >& bactions, int n) const{
	VCS_Function& vcs = *static_cast<VCS_Function*>(evl);

	while(cont.spaces.size()>0){
		int isp = cont.top_space();
		const Space& sp = cont.spaces[isp];

		if(support && corner_support){
			support_faces.clear();
			if(sp.zmin>0){
				bool any_face=false;
				for(const PlacedBlock& b : cont.blocks){
					if(b.zmin>=sp.zmin || b.zmax<sp.zmin || b.xmin>sp.xmax || sp.xmin>b.xmax ||
							b.ymin>sp.ymax || sp.ymin>b.ymax) continue;
					for(const Block::PlacedBox& p : b.block->pboxes){
						if(b.zmin+p.zmax != sp.zmin) continue;
						bool fragile=types[p.type]->fragile;
						support_faces.push_back(Face{b.xmin+p.xmin, b.ymin+p.ymin, b.xmin+p.xmax, b.ymin+p.ymax, fragile});
						any_face = any_face || !fragile;
					}
				}
				//el espacio no tiene base
				if(!any_face) { cont.remove_space(isp); continue; }
			}
		}else if(support){
			support_tops.clear();
			if(sp.zmin>0){
				for(const PlacedBlock& b : cont.blocks)
					if(b.zmax==sp.zmin && b.block->supportive && b.xmin<sp.xmax && sp.xmin<b.xmax &&
							b.ymin<sp.ymax && sp.ymin<b.ymax) support_tops.push_back(&b);
				//el espacio no tiene base
				if(support_tops.empty()) { cont.remove_space(isp); continue; }
			}
		}

		vcs.set_space(*this, sp);

		//las mejores n acciones ordenadas de peor a mejor (en caso de empate,
		//la evaluada primero se considera peor)
		struct Ranked { double eval; const Block* b; long x, y, z; };
		static vector<Ranked> ranked;
		ranked.clear();
		bool any=false;
		for(const Block* b : valid_blocks){
			if(!b->fits(sp.getL(), sp.getW(), sp.getH())) continue;
			long x, y, z;
			if(!locate(*b, sp, x, y, z)) continue;
			any=true;
			//umbral: evaluacion que hay que superar para entrar a las n mejores
			double threshold = ((int)ranked.size()<n)? -HUGE_VAL : ranked.front().eval;
			double eval = vcs.eval(*this, *b, sp, x, y, z, threshold);
			if(eval>0 && ((int)ranked.size()<n || ranked.front().eval < eval)){
				auto pos = upper_bound(ranked.begin(), ranked.end(), eval,
						[](double e, const Ranked& r){ return e < r.eval; });
				ranked.insert(pos, Ranked{eval, b, x, y, z});
				if((int)ranked.size()==n+1) ranked.erase(ranked.begin());
			}
		}

		//el espacio se elimina si no cabe ningun bloque
		if(!any) { cont.remove_space(isp); continue; }

		for(auto it=ranked.rbegin(); it!=ranked.rend(); it++)
			bactions.push_back(new clpAction(*it->b, sp, it->x, it->y, it->z));

		return bactions.size();
	}
	return 0;
}

bool clpState::locate(const Block& b, const Space& sp, long& x, long& y, long& z) const{
	long l=b.getL(), w=b.getW(), h=b.getH();
	sp.get_location(l, w, h, x, y, z);
	if(support) z=sp.zmin;

	if(uld && !uld->adjust(x, z, l, h, sp.xmin, sp.xmax-l, sp.zmin, sp.zmax-h, !support))
		return false;

	//con el criterio de las esquinas puede quedar un hueco bajo una caja: una caja fragil
	//ubicada en el hueco no puede tocar la base de la caja de arriba
	if(corner_support && b.fragile){
		for(const Block::PlacedBox& p : b.pboxes){
			if(!types[p.type]->fragile) continue;
			long top=z+p.zmax, x1=x+p.xmin, y1=y+p.ymin, x2=x+p.xmax, y2=y+p.ymax;
			for(const PlacedBlock& pb : cont.blocks){
				if(pb.zmin>top || pb.zmax<=top || pb.xmin>=x2 || pb.xmax<=x1 || pb.ymin>=y2 || pb.ymax<=y1) continue;
				for(const Block::PlacedBox& q : pb.block->pboxes)
					if(pb.zmin+q.zmin==top && min(x2, pb.xmin+q.xmax)>max(x1, pb.xmin+q.xmin) &&
							min(y2, pb.ymin+q.ymax)>max(y1, pb.ymin+q.ymin)) return false;
			}
		}
	}

	if(support && z>0){
		if(corner_support) return corners_supported(b, x, y, support_faces);
		if(covered_area(support_tops, x, y, l, w) < l*w) return false;
	}
	return true;
}

void clpState::_transition(const Action& action) {
	const clpAction& act = static_cast<const clpAction&>(action);
	const Block& b= act.block;

	//se actualizan las cajas restantes
	for(auto& p : b.boxes) left[p.first] -= p.second;
	weight_left -= b.weight;

	//se inserta el bloque en el contenedor
	long x=act.x, y=act.y, z=act.z;
	cont.insert(b, x, y, z, min_l, min_w, min_h);

	hash += placement_hash(b.id, x, y, z);

	update_valid_blocks();
}

void clpState::update_valid_blocks() {
	valid_blocks.erase(remove_if(valid_blocks.begin(), valid_blocks.end(),
		[this](const Block* b){
			if(b->weight > weight_left + 1e-9) return true;
			for(auto& p : b->boxes)
				if(left[p.first] < p.second) return true;
			return false;
		}), valid_blocks.end());
}

bool clpState::is_feasible() const{
	const vector<PlacedBlock>& B = cont.blocks;
	AABB container(0, 0, 0, cont.getL(), cont.getW(), cont.getH());
	vector<int> used(types.size(), 0);
	long volume=0;
	for(size_t i=0; i<B.size(); i++){
		if(!container.contains(B[i])) return false;
		if(uld && !uld->inside(B[i])) return false;
		for(size_t j=i+1; j<B.size(); j++)
			if(B[i].strict_intersects(B[j])) return false;
		for(auto& p : B[i].block->boxes) used[p.first] += p.second;
		volume += B[i].block->getOccupiedVolume();

		//(con el criterio de las esquinas, el soporte se verifica caja por caja en BSG_ULD)
		if(support && !corner_support && B[i].zmin>0){
			vector<const PlacedBlock*> tops;
			for(const PlacedBlock& b : B)
				if(b.zmax==B[i].zmin && b.block->supportive) tops.push_back(&b);
			if(covered_area(tops, B[i].xmin, B[i].ymin, B[i].getL(), B[i].getW()) < B[i].getL()*B[i].getW())
				return false;
		}
	}
	for(size_t t=0; t<types.size(); t++)
		if(used[t] + left[t] != initial_boxes[t] || left[t] < 0) return false;
	if(uld && get_weight() > uld->max_weight + 1e-6) return false;
	return volume == cont.getOccupiedVolume();
}

double clpState::get_weight() const{
	double w=0.0;
	for(const PlacedBlock& b : cont.blocks) w += b.block->weight;
	return w;
}

void clpState::get_cog(double& x, double& y, double& z) const{
	double w=0.0;
	x=y=z=0.0;
	for(const PlacedBlock& b : cont.blocks){
		double bw=b.block->weight;
		x += bw*(b.xmin+b.block->cog[0]);
		y += bw*(b.ymin+b.block->cog[1]);
		z += bw*(b.zmin+b.block->cog[2]);
		w += bw;
	}
	if(w>0){ x/=w; y/=w; z/=w; }
}

double clpState::cog_violation(long& dx, long& dy) const{
	double cx, cy, cz;
	get_cog(cx, cy, cz);

	//desplazamientos posibles
	long xlo=-cont.getL(), xhi=cont.getL(), ylo=-cont.getW(), yhi=cont.getW();
	for(const PlacedBlock& b : cont.blocks){
		uld->x_shift_range(b, xlo, xhi);
		ylo=max(ylo, -b.ymin);
		yhi=min(yhi, cont.getW()-b.ymax);
	}

	//el desplazamiento mas cercano al que centra la carga
	dx = min(max((long) floor(cont.getL()/2.0 - cx + 0.5), xlo), xhi);
	dy = min(max((long) floor(cont.getW()/2.0 - cy + 0.5), ylo), yhi);

	double ex = max(0.0, fabs(cx+dx - cont.getL()/2.0) - uld->dev[0]) / uld->dev[0];
	double ey = max(0.0, fabs(cy+dy - cont.getW()/2.0) - uld->dev[1]) / uld->dev[1];
	//en z, el CoG debe estar bajo la altura dev[2] (Paquay et al. 2016)
	double ez = max(0.0, cz - uld->dev[2]) / uld->dev[2];
	return ex+ey+ez;
}

void clpState::translate(long dx, long dy){
	for(PlacedBlock& b : cont.blocks){
		b.xmin+=dx; b.xmax+=dx;
		b.ymin+=dy; b.ymax+=dy;
	}
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
			clpState::initial_boxes.push_back(n);

		}
	}

	s->valid_blocks = Block::generate_blocks(clpState::types, s->left, min_fr, max_bl,
			s->cont.getL(), s->cont.getW(), s->cont.getH());
	s->update_min_dim();

	return s;
}

clpState* new_state(const ULD& u, const vector<int>& left, double min_fr, int max_bl){
	clpState* s = new clpState(u.L, u.W, u.H);
	clpState::uld = &u;
	clpState::initial_boxes = left;
	s->left = left;
	s->weight_left = u.max_weight;

	s->valid_blocks = Block::generate_blocks(clpState::types, s->left, min_fr, max_bl,
			u.L, u.W, u.H, clpState::support);
	if(clpState::corner_support) Block::compute_placed_boxes();
	s->update_valid_blocks();
	s->update_min_dim();

	return s;
}

} /* namespace clp */
