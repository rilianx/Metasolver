/*
 * plpState.cpp
 */

#include <cmath>
#include <map>
#include "plpState.h"

using namespace std;

namespace plp {

namespace {

inline uint64_t mix(uint64_t x) {
	x += 0x9e3779b97f4a7c15ull;
	x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ull;
	x = (x ^ (x >> 27)) * 0x94d049bb133111ebull;
	return x ^ (x >> 31);
}

}

PLP_Function::PLP_Function(Placer& placer, double alpha, double beta, double gamma) :
		placer(placer), alpha(alpha), beta(beta), gamma(gamma), vmax(0) {
	for(const Piece& P : placer.inst.pieces) vmax = max(vmax, P.volume);
	h0 = placer.inst.height_lower_bound();
}

double PLP_Function::eval(int type, const Vec3& t, double top) const {
	const Piece& P = placer.inst.pieces[placer.inst.types[type].piece];
	double ztop = t.z + P.bbox.extent().z;
	return alpha*log(P.volume/vmax) - beta*t.z/h0 - gamma*max(0.0, ztop-top)/h0;
}

plpState::plpState(const Instance& inst) : inst(&inst), n_left(0), top(0), volume(0), hash(0x9e3779b97f4a7c15ull) {
	for(const ItemType& t : inst.types) { left.push_back(t.demand); n_left+=t.demand; }
}

double plpState::get_value() const {
	if(n_left>0 || top<=0) return 0.0;
	return volume / (inst->W * inst->D * top);
}

int plpState::get_best_actions(ActionEvaluator* evl, list< Action* >& bactions, int n) const {
	PLP_Function* f = dynamic_cast<PLP_Function*>(evl);
	if(!f) throw runtime_error("plpState: se requiere un PLP_Function");

	//las n mejores acciones (mayor evaluacion primero)
	multimap<double, plpAction> best;
	vector<Vec3> pos;
	for(size_t k=0; k<left.size(); k++){
		if(left[k]==0) continue;
		pos.clear();
		f->placer.candidates(inst->types[k].piece, placed, top, pos);
		for(const Vec3& t : pos){
			double e = f->eval(k, t, top);
			if((int)best.size() < n || e > best.begin()->first){
				best.insert(make_pair(e, plpAction(k, t)));
				if((int)best.size() > n) best.erase(best.begin());
			}
		}
	}
	for(auto it=best.rbegin(); it!=best.rend(); it++)
		bactions.push_back(it->second.clone());
	return best.size();
}

void plpState::_transition(const Action& action) {
	const plpAction& a = dynamic_cast<const plpAction&>(action);
	const Piece& P = inst->pieces[inst->types[a.type].piece];

	items.push_back(Item{a.type, a.t});
	for(const ConvexPolytope& C : P.parts){
		PlacedPart pp;
		pp.part = -1;
		pp.t = a.t;
		pp.bbox.lo = C.get_bbox().lo + a.t;
		pp.bbox.hi = C.get_bbox().hi + a.t;
		placed.push_back(pp);
	}
	//indices globales de las partes (mismo orden que Placer)
	int first=0;
	for(int p=0; p<inst->types[a.type].piece; p++) first += inst->pieces[p].parts.size();
	for(size_t k=0; k<P.parts.size(); k++) placed[placed.size()-P.parts.size()+k].part = first+k;

	left[a.type]--;
	n_left--;
	top = max(top, a.t.z + P.bbox.extent().z);
	volume += P.volume;

	//hash independiente del orden: suma de las ubicaciones (cuantizadas)
	double q = 1e-6 * max(inst->W, inst->D);
	uint64_t h = mix(a.type);
	for(int c=0; c<3; c++) h = mix(h ^ (uint64_t) llround(a.t[c]/q));
	hash += h;
}

Solution plpState::get_solution() const {
	Solution S(*inst);
	for(const Item& it : items){
		Placement p;
		p.type = it.type;
		p.t = it.t;
		S.items.push_back(p);
	}
	return S;
}

} /* namespace plp */
