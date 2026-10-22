/*
 * plpState.cpp
 */

#include <cmath>
#include <map>
#include "plpState.h"
#include "LPBackend.h"

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

double PLP_Function::eval(int p, const Vec3& t, double top) const {
	double ztop = t.z + placer.shape_extent(p).z;
	//volumen por pieza (un pseudobloque no se prefiere solo por ser mas grande)
	double v = placer.shape_volume(p) / placer.shape_pieces(p);
	return alpha*log(v/vmax) - beta*t.z/h0 - gamma*max(0.0, ztop-top)/h0;
}

plpState::plpState(const Instance& inst, double hmax) : inst(&inst), n_left(0), top(0), volume(0), hmax(hmax), hash(0x9e3779b97f4a7c15ull) {
	for(const ItemType& t : inst.types) { left.push_back(t.demand); n_left+=t.demand; }
}

double plpState::get_value() const {
	if(is_knapsack()) return volume / inst->total_volume() - 1e-3*top/hmax;
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
		int p = inst->types[k].piece;
		f->placer.candidates(p, placed, top, pos, hmax);
		for(const Vec3& t : pos){
			double e = f->eval(p, t, top);
			if((int)best.size() < n || e > best.begin()->first){
				best.insert(make_pair(e, plpAction(k, t)));
				if((int)best.size() > n) best.erase(best.begin());
			}
		}
	}

	//pseudobloques
	const vector<Block>& blocks = f->placer.get_blocks();
	for(size_t b=0; b<blocks.size(); b++){
		const Block& B = blocks[b];
		if(left[B.k1]==0 || left[B.k2]==0 || (B.k1==B.k2 && left[B.k1]<2)) continue;
		int p = f->placer.n_pieces() + b;
		pos.clear();
		f->placer.candidates(p, placed, top, pos, hmax);
		for(const Vec3& t : pos){
			double e = f->eval(p, t, top);
			if((int)best.size() < n || e > best.begin()->first){
				best.insert(make_pair(e, plpAction(B.k1, t + B.o1, B.k2, t + B.o2)));
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
	add_item(a.type, a.t);
	if(a.type2 >= 0) add_item(a.type2, a.t2);
}

void plpState::add_item(int type, const Vec3& t) {
	const Piece& P = inst->pieces[inst->types[type].piece];

	items.push_back(Item{type, t});
	//indices globales de las partes (mismo orden que Placer)
	int first=0;
	for(int p=0; p<inst->types[type].piece; p++) first += inst->pieces[p].parts.size();
	for(size_t k=0; k<P.parts.size(); k++){
		PlacedPart pp;
		pp.part = first+k;
		pp.t = t;
		pp.bbox.lo = P.parts[k].get_bbox().lo + t;
		pp.bbox.hi = P.parts[k].get_bbox().hi + t;
		placed.push_back(pp);
	}

	left[type]--;
	n_left--;
	top = max(top, t.z + P.bbox.extent().z);
	volume += P.volume;

	//hash independiente del orden: suma de las ubicaciones (cuantizadas)
	double q = 1e-6 * max(inst->W, inst->D);
	uint64_t h = mix(type);
	for(int c=0; c<3; c++) h = mix(h ^ (uint64_t) llround(t[c]/q));
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

void plpState::get_positions(vector<int>& pieces, vector<Vec3>& t) const {
	pieces.clear();
	t.clear();
	for(const Item& it : items){
		pieces.push_back(inst->types[it.type].piece);
		t.push_back(it.t);
	}
}

void plpState::set_positions(const vector<Vec3>& t) {
	top = 0;
	for(size_t i=0; i<items.size(); i++){
		items[i].t = t[i];
		top = max(top, t[i].z + inst->pieces[inst->types[items[i].type].piece].bbox.extent().z);
	}
	//las partes ubicadas ya no se usan (la solucion esta completa), pero se mantienen consistentes
	placed.clear();
	for(const Item& it : items){
		const Piece& P = inst->pieces[inst->types[it.type].piece];
		int first=0;
		for(int p=0; p<inst->types[it.type].piece; p++) first += inst->pieces[p].parts.size();
		for(size_t k=0; k<P.parts.size(); k++){
			PlacedPart pp;
			pp.part = first+k;
			pp.t = it.t;
			pp.bbox.lo = P.parts[k].get_bbox().lo + it.t;
			pp.bbox.hi = P.parts[k].get_bbox().hi + it.t;
			placed.push_back(pp);
		}
	}
}

double CompactGreedy::run(State& s, double tl, clock_t bt) {
	Greedy::run(s, tl, bt);
	plpState& ps = dynamic_cast<plpState&>(s);
	if(lp && ps.get_n_left()==0 && (tl==0.0 || double(clock()-bt)/CLOCKS_PER_SEC <= tl)){
		vector<int> pieces;
		vector<Vec3> t;
		ps.get_positions(pieces, t);
		static_cast<PLP_Function*>(evl)->placer.compact(pieces, t, *lp, iterations);
		ps.set_positions(t);
	}
	if(ps.is_knapsack() && ps.get_n_left()==0)
		for(SearchStrategy* st : stop_when_complete) st->stop();
	return s.get_value();
}

} /* namespace plp */

