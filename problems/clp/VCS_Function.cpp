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
		long L, long W, long H, double alpha, double beta, double gamma, double p, double delta) :
		alpha(alpha), beta(beta), gamma(gamma), p(p), delta(delta) {

	compute_mX(types, nb_boxes, L, mL, listL, 0);
	compute_mX(types, nb_boxes, W, mW, listW, 1);
	compute_mX(types, nb_boxes, H, mH, listH, 2);
}

//desplazamiento del pseudo-contacto para un largo l
static inline long expansion(double p, long l){
	return (long) ceil(p*(double) l - 0.001);
}

void VCS_Function::set_space(const clpState& s, const Space& sp){
	neighbors.clear();
	if(alpha<=0.0) return;

	//los bloques candidatos tienen dimensiones menores o iguales a las del espacio
	long dx=expansion(p, sp.getL()), dy=expansion(p, sp.getW()), dz=expansion(p, sp.getH());
	AABB region(sp.xmin-dx, sp.ymin-dy, sp.zmin-dz, sp.xmax+dx, sp.ymax+dy, sp.zmax+dz);

	for(const PlacedBlock& b : s.cont.blocks)
		if(b.touches(region)) neighbors.push_back(&b);
}

double VCS_Function::eval(const clpState& s, const Block& b, const Space& sp){

	if(!b.fits(sp.getL(), sp.getW(), sp.getH())) return -1.0;

	double loss=(beta>0.0)? Loss(s.left, b, sp) : 0.0;

	double vol=(delta>0.0)? (double) b.getOccupiedVolume(): 1.0;

	double cs=(alpha>0.0)? CS_p(s, b, sp) : 1.0;

	double n=(gamma>0.0)? (1.0/(double) b.n_boxes) : 1.0;

	return pow(vol, delta)  * pow((1.0-loss),beta) * pow(cs,alpha) * pow(n,gamma);
}

double VCS_Function::CS_p(const clpState& s, const Block& b, const Space& sp){
	long x, y, z;
	sp.get_location(b.getL(), b.getW(), b.getH(), x, y, z);

	long dx=expansion(p, b.getL()), dy=expansion(p, b.getW()), dz=expansion(p, b.getH());

	AABB bb(x, y, z, x+b.getL(), y+b.getW(), z+b.getH());
	AABB bexp(x-dx, y-dy, z-dz, bb.xmax+dx, bb.ymax+dy, bb.zmax+dz);

	long surface=0;
	for(const PlacedBlock* bi : neighbors)
		if(bi->touches(bexp)) surface += surface_in_contact(bb, *bi);

	surface += surface_in_contact_container(bb, s.cont.getL(), s.cont.getW(), s.cont.getH());

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

long VCS_Function::surface_in_contact_container(const AABB& b, long L, long W, long H) const{
	long surface=0;

	if(b.xmin <= p*(double)b.getL()) surface += b.getW() * b.getH();
	if(b.xmax >= L - p*(double)b.getL()) surface += b.getW() * b.getH();

	if(b.ymin <= p*(double)b.getW()) surface += b.getL() * b.getH();
	if(b.ymax >= W - p*(double)b.getW()) surface += b.getL() * b.getH();

	if(b.zmin <= p*(double)b.getH()) surface += b.getL() * b.getW();
	if(b.zmax >= H - p*(double)b.getH()) surface += b.getL() * b.getW();

	return surface;
}

double VCS_Function::Loss(const vector<int>& left, const Block& block, const Space& free_space) const{
	long L=free_space.getL(), W=free_space.getW(), H=free_space.getH();

	long lossL = compute_lossX(left, block, L - block.getL(), mL, listL);
	long lossW = compute_lossX(left, block, W - block.getW(), mW, listW);
	long lossH = compute_lossX(left, block, H - block.getH(), mH, listH);

	long max_vol = mL[L]*mW[W]*mH[H];
	long vloss = max_vol - (L-lossL)*(W-lossW)*(H-lossH);
	return (double) vloss / (double) max_vol;
}

long VCS_Function::compute_lossX(const vector<int>& left, const Block& block, long resX,
		const vector<long>& mX, const vector< vector<int> >& listX) const{
	long maxX = mX[resX];
	while(maxX>0){
		for(int t : listX[maxX])
			if(left[t] > 0 && left[t] - block.count(t) > 0) return resX-maxX;
		maxX = mX[maxX-1];
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

		for(int ii=0; ii<3; ii++){
			long x=0;
			switch(ii){
				case 0: x=box->get(dim, BoxShape::LWH); break;
				case 1: if(box->is_valid(BoxShape::WHL)) x=box->get(dim, BoxShape::WHL); break;
				case 2: if(box->is_valid(BoxShape::HLW)) x=box->get(dim, BoxShape::HLW); break;
			}
			if(x==0) continue;

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
