/*
 * VCS_Function.cpp
 *
 *  Created on: 2 jun. 2017
 *      Author: iaraya
 */

#include <cmath>
#include <algorithm>
#include "VCS_Function.h"
#include "clpState.h"

using namespace std;

namespace clp {

VCS_Function::VCS_Function(const vector<const BoxShape*>& types, const vector<int>& nb_boxes,
		const vector<const Block*>& blocks,
		long L, long W, long H, double alpha, double beta, double gamma, double p, double delta,
		bool fix_knapsack) :
		fix_knapsack(fix_knapsack), alpha(alpha), beta(beta), gamma(gamma), p(p), delta(delta) {

	long dims[3]={L, W, H};
	for(int d=0; d<3; d++){
		LossData& D=ld[d];
		vector< vector<int> > listX;
		compute_mX(types, nb_boxes, dims[d], D.mX, listX, d);
		D.offset.assign(1, 0);
		for(auto& l : listX){
			D.list.insert(D.list.end(), l.begin(), l.end());
			D.offset.push_back(D.list.size());
		}
	}

	int max_id=-1;
	for(const Block* b : blocks) max_id=max(max_id, b->id);
	pow_vol.resize(max_id+1);
	pow_n.resize(max_id+1);
	for(const Block* b : blocks){
		double vol=(delta>0.0)? (double) b->getOccupiedVolume(): 1.0;
		double n=(gamma>0.0)? (1.0/(double) b->n_boxes) : 1.0;
		pow_vol[b->id] = pow(vol, delta);
		pow_n[b->id] = pow(n, gamma);
	}
}

//desplazamiento del pseudo-contacto para un largo l
static inline long expansion(double p, long l){
	return (long) ceil(p*(double) l - 0.001);
}

void VCS_Function::set_space(const clpState& s, const Space& sp){
	bounds = s.cont.regions[sp.region];

	//cota de (1-loss)^beta en el espacio: 1-loss <= volumen del espacio / volumen rellenable
	loss_bound = 1.0;
	if(beta>0.0){
		long max_vol = ld[0].mX[sp.getL()]*ld[1].mX[sp.getW()]*ld[2].mX[sp.getH()];
		loss_bound = (max_vol>0)? pow(max(1.0, (double) sp.getVolume()/(double) max_vol), beta) : HUGE_VAL;
	}

	neighbors.clear();
	if(alpha<=0.0) return;

	//los bloques candidatos tienen dimensiones menores o iguales a las del espacio
	long dx=expansion(p, sp.getL()), dy=expansion(p, sp.getW()), dz=expansion(p, sp.getH());
	AABB region(sp.xmin-dx, sp.ymin-dy, sp.zmin-dz, sp.xmax+dx, sp.ymax+dy, sp.zmax+dz);

	for(const PlacedBlock& b : s.cont.blocks)
		if(b.touches(region)) neighbors.push_back(&b);
}

double VCS_Function::eval(const clpState& s, const Block& b, const Space& sp, long x, long y, long z,
		double threshold){

	if(!b.fits(sp.getL(), sp.getW(), sp.getH())) return -1.0;

	double cs=(alpha>0.0)? CS_p(s, b, x, y, z) : 1.0;
	double pow_cs = pow(cs,alpha);

	//poda: la perdida de volumen solo se calcula si la cota de la evaluacion
	//(con el mayor valor posible de (1-loss)^beta) supera el umbral
	if(pow_vol[b.id] * loss_bound * pow_cs * pow_n[b.id] * (1.0+1e-9) <= threshold) return -1.0;

	double loss=(beta>0.0)? Loss(s.left, b, sp) : 0.0;

	return pow_vol[b.id] * pow((1.0-loss),beta) * pow_cs * pow_n[b.id];
}

double VCS_Function::CS_p(const clpState& s, const Block& b, long x, long y, long z){
	long dx=expansion(p, b.getL()), dy=expansion(p, b.getW()), dz=expansion(p, b.getH());

	AABB bb(x, y, z, x+b.getL(), y+b.getW(), z+b.getH());
	AABB bexp(x-dx, y-dy, z-dz, bb.xmax+dx, bb.ymax+dy, bb.zmax+dz);

	long surface=0;
	for(const PlacedBlock* bi : neighbors)
		if(bi->touches(bexp)) surface += surface_in_contact(bb, *bi);

	surface += surface_in_contact_container(bb, bounds);

	return (double)surface / (double)(bb.getSurface());
}

long VCS_Function::surface_in_contact(const AABB& b, const AABB& bi) const{
	long x_min = std::max(b.xmin, bi.xmin);
	long x_max = std::min(b.xmax, bi.xmax);
	long y_min = std::max(b.ymin, bi.ymin);
	long y_max = std::min(b.ymax, bi.ymax);
	long z_min = std::max(b.zmin, bi.zmin);
	long z_max = std::min(b.zmax, bi.zmax);

	if(y_max>y_min && z_max>z_min){
		if(b.xmax >= bi.xmin - p*(double)b.getL() || bi.xmax >= b.xmin - p*(double)b.getL())
			return (y_max-y_min)*(z_max-z_min);
	}else if(x_max>x_min && z_max>z_min){
		if(b.ymax >= bi.ymin - p*(double)b.getW() || bi.ymax >= b.ymin - p*(double)b.getW())
			return (x_max-x_min)*(z_max-z_min);
	}else if(x_max>x_min && y_max>y_min){
		if(b.zmax >= bi.zmin - p*(double)b.getH() || bi.zmax >= b.zmin - p*(double)b.getH())
			return (x_max-x_min)*(y_max-y_min);
	}
	return 0;
}

long VCS_Function::surface_in_contact_container(const AABB& b, const AABB& c) const{
	long surface=0;

	if(b.xmin - c.xmin <= p*(double)b.getL()) surface += b.getW() * b.getH();
	if(b.xmax >= c.xmax - p*(double)b.getL()) surface += b.getW() * b.getH();

	if(b.ymin - c.ymin <= p*(double)b.getW()) surface += b.getL() * b.getH();
	if(b.ymax >= c.ymax - p*(double)b.getW()) surface += b.getL() * b.getH();

	if(b.zmin - c.zmin <= p*(double)b.getH()) surface += b.getL() * b.getW();
	if(b.zmax >= c.zmax - p*(double)b.getH()) surface += b.getL() * b.getW();

	return surface;
}

double VCS_Function::Loss(const vector<int>& left, const Block& block, const Space& free_space) const{
	long L=free_space.getL(), W=free_space.getW(), H=free_space.getH();

	long lossL = compute_lossX(0, left, block, L - block.getL());
	long lossW = compute_lossX(1, left, block, W - block.getW());
	long lossH = compute_lossX(2, left, block, H - block.getH());

	long max_vol = ld[0].mX[L]*ld[1].mX[W]*ld[2].mX[H];
	long vloss = max_vol - (L-lossL)*(W-lossW)*(H-lossH);
	return (double) vloss / (double) max_vol;
}

long VCS_Function::compute_lossX(int d, const vector<int>& left, const Block& block, long resX) const{
	const LossData& D=ld[d];
	long maxX = D.mX[resX];
	while(maxX>0){
		for(int k=D.offset[maxX]; k<D.offset[maxX+1]; k++){
			int t=D.list[k];
			if(left[t] > 0 && left[t] - block.count(t) > 0) return resX-maxX;
		}
		maxX = D.mX[maxX-1];
	}
	return resX;
}

void VCS_Function::compute_mX(const vector<const BoxShape*>& types, const vector<int>& nb_boxes,
		long X, vector<long>& mX, vector< vector<int> >& listX, int dim){
	vector<bool> flag(X+1, false);
	flag[0]=true;
	mX.assign(X+1, 0);
	listX.assign(X+1, vector<int>());

	for(const BoxShape* box : types){
		int n=nb_boxes[box->type];
		if(n <= 0) continue;

		//largos de la caja en la dimension dim
		vector<long> lengths;
		if(fix_knapsack){
			//todos los largos distintos de las orientaciones permitidas
			for(int o=0; o<6; o++)
				if(box->is_valid(o) && find(lengths.begin(), lengths.end(), box->get(dim, o))==lengths.end())
					lengths.push_back(box->get(dim, o));
		}else{
			//version original: solo las orientaciones LWH, WHL y HLW
			lengths.push_back(box->get(dim, BoxShape::LWH));
			if(box->is_valid(BoxShape::WHL)) lengths.push_back(box->get(dim, BoxShape::WHL));
			if(box->is_valid(BoxShape::HLW)) lengths.push_back(box->get(dim, BoxShape::HLW));
		}

		for(long x : lengths){

			for(long i=0; i<=X-x; i++){
				if(!flag[i]) continue;
				for(long j=min((long)n,(X-i)/x); j>=1; j--){
					flag[i+x*j]=true;
					vector<int>& l = listX[i+x*j];
					if(l.empty() || l.back()!=box->type) l.push_back(box->type);
				}
			}
		}
	}

	for(long i=0; i<=X; i++){
		if(flag[i]) mX[i]=i;
		else mX[i]=mX[i-1];
	}
}

} /* namespace clp */
