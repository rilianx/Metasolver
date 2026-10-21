/*
 * LNS.cpp
 */

#include <cmath>
#include <algorithm>
#include <iostream>
#include "LNS.h"
#include "BSG.h"

using namespace std;

namespace clp {

clpLNS::clpLNS(const clpState& s0, ActionEvaluator* evl, SearchStrategy& greedy, Shape shape, Center center,
		Repair repair, CpSolver* solver, ExactParams exact, int beams, double k0, double kmax, double growth,
		unsigned int seed) :
	s0(s0), evl(evl), greedy(greedy), shape(shape), center(center), repair(repair), solver(solver),
	exact(exact), beams(beams), k0(k0), kmax(kmax), growth(growth), rng(seed), iterations(0),
	exact_improvements(0), has_void(false), too_large(false) {

	for(const Block* b : s0.valid_blocks)
		if(b->n_boxes==1)
			single_blocks[make_tuple(b->boxes.begin()->first, b->l, b->w, b->h)] = b;
}

clpState* clpLNS::rebuild(const vector<PlacedBlock>& blocks) const{
	clpState* s = static_cast<clpState*>(s0.clone());
	long L=s0.cont.getL(), W=s0.cont.getW(), H=s0.cont.getH();
	for(const PlacedBlock& pb : blocks){
		//el espacio tiene exactamente el tamano del bloque, por lo que el bloque
		//queda en la misma posicion
		clpAction action(*pb.block, Space(pb, L, W, H));
		s->transition(action);
	}
	return s;
}

void clpLNS::destroy(const clpState& s, int k, vector<PlacedBlock>& keep, vector<PlacedBlock>& removed){
	double dims[3]={(double) s0.cont.getL(), (double) s0.cont.getW(), (double) s0.cont.getH()};

	double c[3];
	has_void = false;
	if(center==VOID){
		//centro de un espacio libre maximal elegido con probabilidad proporcional a
		//su volumen (los espacios de la solucion se recalculan reconstruyendola)
		clpState* r = rebuild(s.cont.blocks);
		const vector<Space>& sp = r->cont.spaces;
		if(sp.empty()) for(int i=0; i<3; i++) c[i]=dims[i]/2.0;
		else{
			vector<double> w;
			for(const Space& e : sp) w.push_back((double) e.volume);
			discrete_distribution<int> pick(w.begin(), w.end());
			const Space& e = sp[pick(rng)];
			c[0]=(e.xmin+e.xmax)/2.0; c[1]=(e.ymin+e.ymax)/2.0; c[2]=(e.zmin+e.zmax)/2.0;
			void_space = e; has_void = true;
		}
		delete r;
	}else{
		//centro del contenedor desplazado aleatoriamente (a lo mas un 10% de cada dimension)
		uniform_real_distribution<double> u(-0.1, 0.1);
		for(int i=0; i<3; i++) c[i] = dims[i]*(0.5 + u(rng));
	}

	const vector<PlacedBlock>& B = s.cont.blocks;
	vector< pair<double,int> > dist;
	for(size_t i=0; i<B.size(); i++){
		double p[3]={(B[i].xmin+B[i].xmax)/2.0, (B[i].ymin+B[i].ymax)/2.0, (B[i].zmin+B[i].zmax)/2.0};
		double d = fabs(p[0]-c[0])/dims[0];
		if(shape==BOX)
			for(int j=1; j<3; j++) d = max(d, fabs(p[j]-c[j])/dims[j]);
		dist.push_back(make_pair(d, i));
	}
	sort(dist.begin(), dist.end());

	vector<bool> rem(B.size(), false);
	for(int i=0; i<k && i<(int)dist.size(); i++) rem[dist[i].second]=true;

	//se mantiene el orden original de los bloques
	keep.clear(); removed.clear();
	for(size_t i=0; i<B.size(); i++)
		if(rem[i]) removed.push_back(B[i]);
		else keep.push_back(B[i]);
}

clpState* clpLNS::bsg_repair(const vector<PlacedBlock>& keep, double tl, clock_t bt){
	//el BSG libera el estado parcial
	clpState* partial = rebuild(keep);
	BSG bsg(evl, greedy, beams, 0.0, 0, false);
	bsg.run(*partial, tl, bt);
	const State* r = bsg.get_best_state();
	return r? static_cast<clpState*>(r->clone()) : NULL;
}

clpState* clpLNS::exact_repair(const vector<PlacedBlock>& keep, const vector<PlacedBlock>& removed,
		double tl, clock_t bt){
	too_large=false;
	double remaining = tl - double(clock()-bt)/double(CLOCKS_PER_SEC);
	if(!solver || remaining <= 0.0) return NULL;

	//region: caja envolvente de los bloques eliminados y del espacio vacio
	//que se uso como centro de la destruccion
	AABB R = removed[0];
	if(has_void){
		R.xmin=min(R.xmin, void_space.xmin); R.ymin=min(R.ymin, void_space.ymin); R.zmin=min(R.zmin, void_space.zmin);
		R.xmax=max(R.xmax, void_space.xmax); R.ymax=max(R.ymax, void_space.ymax); R.zmax=max(R.zmax, void_space.zmax);
	}
	long removed_volume=0;
	for(const PlacedBlock& b : removed){
		R.xmin=min(R.xmin, b.xmin); R.ymin=min(R.ymin, b.ymin); R.zmin=min(R.zmin, b.zmin);
		R.xmax=max(R.xmax, b.xmax); R.ymax=max(R.ymax, b.ymax); R.zmax=max(R.zmax, b.zmax);
		removed_volume += b.block->getOccupiedVolume();
	}

	//obstaculos: bloques que se mantienen e intersectan la region
	vector<AABB> obstacles;
	long free_volume = R.getVolume();
	for(const PlacedBlock& b : keep)
		if(b.strict_intersects(R)){
			obstacles.push_back(b);
			free_volume -= (min(b.xmax,R.xmax)-max(b.xmin,R.xmin)) * (min(b.ymax,R.ymax)-max(b.ymin,R.ymin))
					* (min(b.zmax,R.zmax)-max(b.zmin,R.zmin));
		}

	//cajas de los bloques eliminados (solucion inicial del modelo)
	vector<Block::BoxPlacement> current;
	for(const PlacedBlock& b : removed) b.block->get_boxes(b.xmin, b.ymin, b.zmin, current);
	if((int)current.size() > exact.max_boxes) { too_large=true; return NULL; }

	clpState* partial = rebuild(keep);

	//cajas candidatas: todas las de los bloques eliminados y, si hay cupo, otras
	//cajas disponibles (primero de los tipos eliminados, luego por volumen decreciente)
	vector<int> need(clpState::types.size(), 0);
	for(auto& bp : current) need[bp.type]++;

	struct Orientation { long l, w, h; };
	struct Candidate { int type; vector<Orientation> ors; };
	vector<int> order;
	for(const BoxShape* box : clpState::types) order.push_back(box->type);
	sort(order.begin(), order.end(), [&](int a, int b){
		if((need[a]>0)!=(need[b]>0)) return need[a]>0;
		return clpState::types[a]->getVolume() > clpState::types[b]->getVolume();
	});

	int extra_budget = exact.max_boxes - current.size();
	long extra_volume = free_volume - removed_volume;
	vector<Candidate> cand;
	long cand_volume=0;
	for(int t : order){
		const BoxShape* box = clpState::types[t];
		if(partial->left[t]<=0) continue;
		Candidate c; c.type=t;
		for(int o=0; o<6; o++){
			if(!box->is_valid(o)) continue;
			Orientation d{box->getL(o), box->getW(o), box->getH(o)};
			if(d.l>R.getL() || d.w>R.getW() || d.h>R.getH()) continue;
			bool repeated=false;
			for(auto& e : c.ors) if(e.l==d.l && e.w==d.w && e.h==d.h) repeated=true;
			if(!repeated) c.ors.push_back(d);
		}
		if(c.ors.empty()) continue;
		long extra = min((long) partial->left[t] - need[t], extra_volume/box->getVolume());
		extra = max(0L, min(extra, (long) extra_budget));
		extra_budget -= extra;
		for(long i=0; i<need[t]+extra; i++){
			cand.push_back(c);
			cand_volume += box->getVolume();
		}
	}

	//no se puede mejorar el volumen eliminado
	if(cand_volume <= removed_volume) { delete partial; return NULL; }

	//modelo
	const int64_t INF=1LL<<40;
	CpModel m;
	int n=cand.size();
	vector<int> p(n), x(n), y(n), z(n), l(n), w(n), h(n);
	vector< vector<int> > o(n);
	vector< vector<int> > sep_obs(n);   //separacion con cada obstaculo (6 por obstaculo)
	for(int i=0; i<n; i++){
		p[i]=m.new_bool();
		x[i]=m.new_var(R.xmin, R.xmax); y[i]=m.new_var(R.ymin, R.ymax); z[i]=m.new_var(R.zmin, R.zmax);
		long ml=0, mw=0, mh=0;
		for(auto& d : cand[i].ors) { ml=max(ml,d.l); mw=max(mw,d.w); mh=max(mh,d.h); }
		l[i]=m.new_var(0, ml); w[i]=m.new_var(0, mw); h[i]=m.new_var(0, mh);

		//una orientacion si la caja esta presente; dimensiones segun la orientacion
		vector<int> ov{p[i]}, lv{l[i]}, wv{w[i]}, hv{h[i]};
		vector<int64_t> oc{-1}, lc{-1}, wc{-1}, hc{-1};
		for(auto& d : cand[i].ors){
			int b=m.new_bool();
			o[i].push_back(b);
			ov.push_back(b); oc.push_back(1);
			lv.push_back(b); lc.push_back(d.l);
			wv.push_back(b); wc.push_back(d.w);
			hv.push_back(b); hc.push_back(d.h);
		}
		m.add_linear(ov, oc, 0, 0);
		m.add_linear(lv, lc, 0, 0);
		m.add_linear(wv, wc, 0, 0);
		m.add_linear(hv, hc, 0, 0);

		//dentro de la region
		m.add_linear({x[i], l[i]}, {1,1}, R.xmin, R.xmax, {p[i]});
		m.add_linear({y[i], w[i]}, {1,1}, R.ymin, R.ymax, {p[i]});
		m.add_linear({z[i], h[i]}, {1,1}, R.zmin, R.zmax, {p[i]});

		//sin interseccion con los obstaculos
		for(const AABB& ob : obstacles){
			vector<int> s;
			for(int k=0; k<6; k++) s.push_back(m.new_bool());
			m.add_linear({x[i], l[i]}, {1,1}, -INF, ob.xmin, {s[0]});
			m.add_linear({x[i]}, {1}, ob.xmax, INF, {s[1]});
			m.add_linear({y[i], w[i]}, {1,1}, -INF, ob.ymin, {s[2]});
			m.add_linear({y[i]}, {1}, ob.ymax, INF, {s[3]});
			m.add_linear({z[i], h[i]}, {1,1}, -INF, ob.zmin, {s[4]});
			m.add_linear({z[i]}, {1}, ob.zmax, INF, {s[5]});
			m.add_bool_or(s, {p[i]});
			sep_obs[i].insert(sep_obs[i].end(), s.begin(), s.end());
		}

		//las copias de un mismo tipo se usan en orden (simetria)
		if(i>0 && cand[i].type==cand[i-1].type)
			m.add_linear({p[i-1], p[i]}, {1,-1}, 0, 1);
	}

	//sin interseccion entre cajas
	vector< vector<int> > sep(n*n);
	for(int i=0; i<n; i++)
		for(int j=i+1; j<n; j++){
			vector<int> s;
			for(int k=0; k<6; k++) s.push_back(m.new_bool());
			m.add_linear({x[i], l[i], x[j]}, {1,1,-1}, -INF, 0, {s[0]});
			m.add_linear({x[j], l[j], x[i]}, {1,1,-1}, -INF, 0, {s[1]});
			m.add_linear({y[i], w[i], y[j]}, {1,1,-1}, -INF, 0, {s[2]});
			m.add_linear({y[j], w[j], y[i]}, {1,1,-1}, -INF, 0, {s[3]});
			m.add_linear({z[i], h[i], z[j]}, {1,1,-1}, -INF, 0, {s[4]});
			m.add_linear({z[j], h[j], z[i]}, {1,1,-1}, -INF, 0, {s[5]});
			m.add_bool_or(s, {p[i], p[j]});
			sep[i*n+j]=s;
		}

	vector<int64_t> vol;
	for(int i=0; i<n; i++) vol.push_back(clpState::types[cand[i].type]->getVolume());
	m.maximize(p, vol);

	//solucion inicial: las cajas de los bloques eliminados
	vector<int> assigned(n, -1);
	{
		vector<bool> used(n, false);
		for(size_t k=0; k<current.size(); k++)
			for(int i=0; i<n; i++)
				if(!used[i] && cand[i].type==current[k].type) { used[i]=true; assigned[i]=k; break; }
	}
	for(int i=0; i<n; i++){
		bool present = assigned[i]>=0;
		const Block::BoxPlacement* bp = present? &current[assigned[i]] : NULL;
		m.add_hint(p[i], present);
		m.add_hint(x[i], present? bp->x : R.xmin);
		m.add_hint(y[i], present? bp->y : R.ymin);
		m.add_hint(z[i], present? bp->z : R.zmin);
		m.add_hint(l[i], present? bp->l : 0);
		m.add_hint(w[i], present? bp->w : 0);
		m.add_hint(h[i], present? bp->h : 0);
		for(size_t k=0; k<cand[i].ors.size(); k++){
			const Orientation& d = cand[i].ors[k];
			m.add_hint(o[i][k], present && d.l==bp->l && d.w==bp->w && d.h==bp->h);
		}
		for(size_t q=0; q<obstacles.size(); q++){
			const AABB& ob = obstacles[q];
			const vector<int>& s = sep_obs[i];
			bool v[6]={false,false,false,false,false,false};
			if(present){
				v[0] = bp->x+bp->l <= ob.xmin; v[1] = bp->x >= ob.xmax;
				v[2] = bp->y+bp->w <= ob.ymin; v[3] = bp->y >= ob.ymax;
				v[4] = bp->z+bp->h <= ob.zmin; v[5] = bp->z >= ob.zmax;
			}
			for(int k=0; k<6; k++) m.add_hint(s[6*q+k], v[k]);
		}
	}
	for(int i=0; i<n; i++)
		for(int j=i+1; j<n; j++){
			bool v[6]={false,false,false,false,false,false};
			if(assigned[i]>=0 && assigned[j]>=0){
				const Block::BoxPlacement& a = current[assigned[i]];
				const Block::BoxPlacement& b = current[assigned[j]];
				v[0] = a.x+a.l <= b.x; v[1] = b.x+b.l <= a.x;
				v[2] = a.y+a.w <= b.y; v[3] = b.y+b.w <= a.y;
				v[4] = a.z+a.h <= b.z; v[5] = b.z+b.h <= a.z;
			}
			for(int k=0; k<6; k++) m.add_hint(sep[i*n+j][k], v[k]);
		}

	vector<int64_t> sol;
	CpSolver::Status st = solver->solve(m, min(exact.max_time, remaining), exact.workers, sol);
	if(st!=CpSolver::FEASIBLE && st!=CpSolver::OPTIMAL) { delete partial; return NULL; }

	long new_volume=0;
	for(int i=0; i<n; i++) if(sol[p[i]]) new_volume += vol[i];
	if(new_volume <= removed_volume) { delete partial; return NULL; }

	//se ubican las cajas (como bloques de una caja) y se completa con greedy
	long L=s0.cont.getL(), W=s0.cont.getW(), H=s0.cont.getH();
	for(int i=0; i<n; i++){
		if(!sol[p[i]]) continue;
		auto it = single_blocks.find(make_tuple(cand[i].type, (long) sol[l[i]], (long) sol[w[i]], (long) sol[h[i]]));
		if(it==single_blocks.end()) { cerr << "ERROR: exact repair, unknown box orientation" << endl; exit(1); }
		AABB a(sol[x[i]], sol[y[i]], sol[z[i]], sol[x[i]]+sol[l[i]], sol[y[i]]+sol[w[i]], sol[z[i]]+sol[h[i]]);
		clpAction action(*it->second, Space(a, L, W, H));
		partial->transition(action);
	}
	greedy.run(*partial, tl, bt);
	return partial;
}

clpState* clpLNS::run(const clpState& s, double tl, clock_t bt){
	auto time = [bt](){ return double(clock()-bt)/double(CLOCKS_PER_SEC); };

	clpState* best = static_cast<clpState*>(s.clone());
	double kf=k0;
	vector<PlacedBlock> keep, removed;

	while(time() < tl){
		//destruccion
		int n = best->cont.blocks.size();
		int k = max(2, (int) lround(kf*n));
		if(k>=n) break;
		destroy(*best, k, keep, removed);

		//reparacion
		clpState* r = NULL;
		bool by_exact = false;
		if(repair!=EXACT_REPAIR) r = bsg_repair(keep, tl, bt);
		if(repair!=BSG_REPAIR && (!r || r->get_value() <= best->get_value())){
			clpState* e = exact_repair(keep, removed, tl, bt);
			if(e && (!r || e->get_value() > r->get_value())) { delete r; r = e; by_exact = true; }
			else delete e;
		}
		iterations++;

		//la region es demasiado grande para la reparacion exacta
		if(repair==EXACT_REPAIR && too_large) { delete r; kf=k0; continue; }

		if(!r || r->get_value() < best->get_value()) { delete r; kf=min(kf*growth, kmax); continue; }

		//se aceptan soluciones iguales (diversificacion), la cantidad de bloques
		//eliminados solo vuelve a su valor inicial si hay mejora
		if(r->get_value() > best->get_value()){
			kf=k0;
			if(by_exact) exact_improvements++;
			cout << "[LNS] new best_solution_found (" << time() << "): " << r->get_value()
					<< " (iteration " << iterations << (by_exact? ", exact" : "") << ")" << endl;
		}else
			kf=min(kf*growth, kmax);

		delete best;
		best = r;
	}

	return best;
}

} /* namespace clp */
