/*
 * Placer.cpp
 */

#include <algorithm>
#include <set>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>
#include "Placer.h"
#include "LPBackend.h"

using namespace std;

namespace plp {

Placer::Placer(const Instance& inst, int grid, double slide, double eps) :
		inst(inst), grid(max(grid,2)), slide(slide), eps(eps), seed(0) {
	double scale = max(max(inst.W, inst.D), inst.height_lower_bound());
	tol = 1e-9*scale;
	for(const Piece& P : inst.pieces){
		piece_parts.push_back(vector<int>());
		shape_parts.push_back(vector<ShapePart>());
		for(const ConvexPolytope& C : P.parts){
			piece_parts.back().push_back(parts.size());
			shape_parts.back().push_back(ShapePart{(int) parts.size(), Vec3()});
			parts.push_back(&C);
		}
		shape_box.push_back(P.bbox);
		shape_vol.push_back(P.volume);
		shape_npieces.push_back(1);
	}
}

const ConvexPolytope& Placer::nfp(int a, int b) {
	auto it = nfps.find(make_pair(a,b));
	if(it!=nfps.end()) return it->second;
	vector<Vec3> pts;
	for(const Vec3& vb : parts[b]->get_vertices())
		for(const Vec3& va : parts[a]->get_vertices())
			pts.push_back(vb - va);
	return nfps.insert(make_pair(make_pair(a,b), ConvexPolytope::hull(pts))).first->second;
}

bool Placer::feasible(int p, const Vec3& t, const vector<PlacedPart>& placed) const {
	for(const ShapePart& sp : shape_parts[p]){
		ConvexPolytope A = parts[sp.part]->translated(t + sp.off);
		for(const PlacedPart& B : placed){
			if(!A.get_bbox().overlaps(B.bbox, eps)) continue;
			if(penetration_depth(A, parts[B.part]->translated(B.t), eps) > eps) return false;
		}
	}
	return true;
}

Vec3 Placer::descend(int p, Vec3 t, const Vec3& c, const vector<PlacedPart>& placed) {
	Vec3 e = shape_box[p].extent();
	double delta = max(max(e.x, e.y), e.z);
	Vec3 g = -c / c.norm();

	for(int round=0; round<100; round++){
		//region de confianza: caja alrededor de t dentro del contenedor (z no aumenta)
		Box3 R;
		R.lo = Vec3(max(0.0, t.x-delta), max(0.0, t.y-delta), max(0.0, t.z-delta));
		R.hi = Vec3(min(inst.W-e.x, t.x+delta), min(inst.D-e.y, t.y+delta), t.z);

		lp.clear();
		for(int k=0; k<3; k++){
			Vec3 u; u[k]=1;
			lp.add(u, R.hi[k]);
			lp.add(-u, -R.lo[k]);
		}

		for(const ShapePart& sp : shape_parts[p]){
			int a = sp.part;
			Box3 ab;
			ab.lo = parts[a]->get_bbox().lo + sp.off;
			ab.hi = parts[a]->get_bbox().hi + sp.off;
			for(const PlacedPart& B : placed){
				//caja de las traslaciones t para las que A y B pueden solaparse
				Box3 nb;
				nb.lo = B.bbox.lo - ab.hi;
				nb.hi = B.bbox.hi - ab.lo;
				if(!nb.overlaps(R, -tol)) continue;

				const ConvexPolytope& N = nfp(a, B.part);
				//la parte a esta en t + off
				Vec3 rel = t + sp.off - B.t;
				//faceta satisfecha por t que permite el mayor avance en la direccion de descenso g:
				//holgura/(-n.g) si n.g < 0 (infinito si n.g >= 0, se prefiere la de mayor n.g)
				int best=-1, worst=-1;
				double best_step=-1, best_g=0, worst_s=-numeric_limits<double>::max();
				const vector<Plane>& F = N.get_facets();
				for(int f=0; f<(int)F.size(); f++){
					double s = dot(F[f].n, rel) - F[f].d;
					if(s > worst_s) { worst_s=s; worst=f; }
					if(s < -tol) continue;
					double ng = dot(F[f].n, g);
					double step = (ng >= -1e-12)? numeric_limits<double>::max() : max(0.0,s)/(-ng);
					if(step > best_step || (step==best_step && ng > best_g)) { best_step=step; best_g=ng; best=f; }
				}
				if(best<0) best=worst;
				//n.(t + off - tB) >= d  <=>  -n.t <= -d + n.(off - tB)
				lp.add(-F[best].n, -F[best].d + dot(F[best].n, sp.off - B.t));
			}
		}

		Vec3 t2 = lp.minimize(c, t);
		if(dot(c, t2) >= dot(c, t) - tol) break;
		t = t2;
	}
	return t;
}

void Placer::generate_blocks(int max_blocks, int per_pair, double max_waste, int max_eval, bool by_box, double min_gain) {
	if(!blocks.empty()) throw runtime_error("generate_blocks: los bloques ya fueron generados");

	//vertices y volumen de la envolvente convexa de cada pieza
	vector< vector<Vec3> > pv(inst.pieces.size());
	vector<double> hull_vol(inst.pieces.size());
	for(size_t p=0; p<inst.pieces.size(); p++){
		for(int a : piece_parts[p])
			for(const Vec3& v : parts[a]->get_vertices()) pv[p].push_back(v);
		hull_vol[p] = ConvexPolytope::hull(pv[p]).get_volume();
	}

	vector<Block> all;
	int nt = inst.types.size();
	for(int k1=0; k1<nt; k1++)
		for(int k2=k1; k2<nt; k2++){
			if(k1==k2 && inst.types[k1].demand<2) continue;
			int p1 = inst.types[k1].piece, p2 = inst.types[k2].piece;
			const Piece& P1 = inst.pieces[p1];
			const Piece& P2 = inst.pieces[p2];
			double vol = P1.volume + P2.volume;

			//p1 fija en el origen; p2 en r se solapa con p1 si r esta en el interior de NFP(parte de p2, parte de p1)
			vector<PlacedPart> fixed;
			for(int b : piece_parts[p1]) fixed.push_back(PlacedPart{b, Vec3(), parts[b]->get_bbox()});

			//posiciones relativas candidatas: vertices de los NFP y, para cada faceta n.r <= d,
			//los puntos de su plano sobre los ejes (r = (d/n_i) e_i: deslizar p2 junto a p1 a lo largo de un eje)
			vector<Vec3> rs;
			for(int a : piece_parts[p2])
				for(int b : piece_parts[p1]){
					const ConvexPolytope& N = nfp(a, b);
					for(const Vec3& r : N.get_vertices()) rs.push_back(r);
					for(const Plane& f : N.get_facets())
						for(int i=0; i<3; i++)
							if(fabs(f.n[i]) > 1e-9) { Vec3 r; r[i] = f.d/f.n[i]; rs.push_back(r); }
				}

			//se ordenan por el volumen de la caja del bloque (filtro barato)
			vector< pair<double,Vec3> > cand;
			for(const Vec3& r : rs){
				Box3 bb = P1.bbox;
				Box3 b2; b2.lo = P2.bbox.lo + r; b2.hi = P2.bbox.hi + r;
				bb.add(b2);
				Vec3 e = bb.extent();
				if(e.x > inst.W + eps || e.y > inst.D + eps) continue;
				if(by_box && bb.volume() > (1-min_gain)*(P1.bbox.volume() + P2.bbox.volume())) continue;
				cand.push_back(make_pair(bb.volume(), r));
			}
			sort(cand.begin(), cand.end(), [](const pair<double,Vec3>& x, const pair<double,Vec3>& y){ return x.first < y.first; });

			//hueco agregado = envolvente(bloque) - envolvente(p1) - envolvente(p2), para max_eval candidatos factibles
			vector< pair<double,Vec3> > ok;
			vector<Vec3> seen;
			for(const auto& c : cand){
				if((int)seen.size() >= ((by_box)? per_pair : max_eval)) break;
				const Vec3& r = c.second;
				//con piezas iguales, r y -r forman el mismo bloque
				bool repeated=false;
				for(const Vec3& u : seen)
					if((u-r).norm() <= 1e3*tol || (k1==k2 && (u+r).norm() <= 1e3*tol)) { repeated=true; break; }
				if(repeated || !feasible(p2, r, fixed)) continue;
				seen.push_back(r);

				vector<Vec3> pts = pv[p1];
				for(const Vec3& v : pv[p2]) pts.push_back(v + r);
				double waste = (ConvexPolytope::hull(pts).get_volume() - hull_vol[p1] - hull_vol[p2]) / vol;
				if(by_box || waste <= max_waste) ok.push_back(make_pair(waste, r));
			}
			//por caja: en el orden de la caja (ya ordenados); por hueco: menor hueco primero
			if(!by_box) sort(ok.begin(), ok.end(), [](const pair<double,Vec3>& x, const pair<double,Vec3>& y){ return x.first < y.first; });

			for(int k=0; k<(int)ok.size() && k<per_pair; k++){
				const Vec3& r = ok[k].second;
				Box3 bb = P1.bbox;
				Box3 b2; b2.lo = P2.bbox.lo + r; b2.hi = P2.bbox.hi + r;
				bb.add(b2);
				Block B;
				B.k1 = k1; B.k2 = k2;
				B.o1 = -bb.lo;
				B.o2 = r - bb.lo;
				B.volume = vol;
				B.bbox.lo = Vec3();
				B.bbox.hi = bb.extent();
				B.waste = ok[k].first;
				B.density = vol / bb.volume();
				all.push_back(B);
			}
		}

	if(by_box) sort(all.begin(), all.end(), [](const Block& x, const Block& y){ return x.density > y.density; });
	else sort(all.begin(), all.end(), [](const Block& x, const Block& y){ return x.waste < y.waste; });
	if((int)all.size() > max_blocks) all.resize(max_blocks);
	blocks = all;

	for(const Block& B : blocks){
		shape_parts.push_back(vector<ShapePart>());
		for(int a : piece_parts[inst.types[B.k1].piece]) shape_parts.back().push_back(ShapePart{a, B.o1});
		for(int a : piece_parts[inst.types[B.k2].piece]) shape_parts.back().push_back(ShapePart{a, B.o2});
		shape_box.push_back(B.bbox);
		shape_vol.push_back(B.volume);
		shape_npieces.push_back(2);
	}
}

void Placer::candidates(int p, const vector<PlacedPart>& placed, double top, vector<Vec3>& out, double hmax) {
	Vec3 e = shape_box[p].extent();
	if(e.x > inst.W + eps || e.y > inst.D + eps || e.z > hmax + eps) return;
	double X = max(0.0, inst.W - e.x), Y = max(0.0, inst.D - e.y);

	for(int i=0; i<grid; i++)
		for(int j=0; j<grid; j++){
			Vec3 t0(X*i/(grid-1), Y*j/(grid-1), top);
			if(seed){
				uniform_real_distribution<double> u(0.0, 100.0*tol);
				t0.x = min(X, t0.x + ((i==0)? u(rng) : -u(rng)));
				t0.y = min(Y, t0.y + ((j==0)? u(rng) : -u(rng)));
				t0.x = max(0.0, t0.x); t0.y = max(0.0, t0.y);
			}
			//gravedad y deslizamiento hacia las paredes mas cercanas
			Vec3 c((t0.x <= X/2)? slide : -slide, (t0.y <= Y/2)? slide : -slide, 1.0);
			Vec3 t = descend(p, t0, c, placed);

			//pequenas violaciones numericas de las cotas del contenedor
			t.x = min(max(t.x, 0.0), X);
			t.y = min(max(t.y, 0.0), Y);
			t.z = max(t.z, 0.0);

			//la pieza parte sobre la carga: si no baja lo suficiente, no cabe en el contenedor
			if(t.z + e.z > hmax + eps) continue;

			bool repeated=false;
			for(const Vec3& u : out) if((u-t).norm() <= 1e3*tol) { repeated=true; break; }
			if(repeated || !feasible(p, t, placed)) continue;
			out.push_back(t);
		}
}

void Placer::place_all(const vector<int>& pieces, const vector<Vec3>& t, int skip, vector<PlacedPart>& out) const {
	out.clear();
	for(size_t i=0; i<pieces.size(); i++){
		if((int)i==skip) continue;
		for(int k : piece_parts[pieces[i]]){
			PlacedPart pp;
			pp.part = k;
			pp.t = t[i];
			pp.bbox.lo = parts[k]->get_bbox().lo + t[i];
			pp.bbox.hi = parts[k]->get_bbox().hi + t[i];
			out.push_back(pp);
		}
	}
}

double Placer::improve(const vector<int>& pieces, vector<Vec3>& t, int max_passes) {
	int n = pieces.size();
	auto height = [&](int skip) {
		double h=0;
		for(int i=0; i<n; i++) if(i!=skip) h = max(h, t[i].z + inst.pieces[pieces[i]].bbox.extent().z);
		return h;
	};

	double H = height(-1);
	vector<PlacedPart> others;
	vector<Vec3> cand;
	for(int pass=0; pass<max_passes; pass++){
		//de abajo hacia arriba
		vector<int> order(n);
		for(int i=0; i<n; i++) order[i]=i;
		sort(order.begin(), order.end(), [&](int a, int b){ return t[a].z < t[b].z; });

		for(int i : order){
			int p = pieces[i];
			double X = max(0.0, inst.W - inst.pieces[p].bbox.extent().x);
			double Y = max(0.0, inst.D - inst.pieces[p].bbox.extent().y);
			place_all(pieces, t, i, others);

			//compactacion: gravedad y deslizamiento hacia la pared mas cercana
			Vec3 c((t[i].x <= X/2)? slide : -slide, (t[i].y <= Y/2)? slide : -slide, 1.0);
			Vec3 u = descend(p, t[i], c, others);
			if(dot(c,u) < dot(c,t[i]) - tol && feasible(p, u, others)) t[i] = u;

			//reubicacion de las piezas que definen la altura
			double ztop = t[i].z + inst.pieces[p].bbox.extent().z;
			if(ztop < H - tol) continue;
			cand.clear();
			candidates(p, others, height(i), cand);
			Vec3 best = t[i];
			for(const Vec3& v : cand)
				if(v.z < best.z - tol) best = v;
			t[i] = best;
		}

		double H2 = height(-1);
		if(H2 >= H - tol) { H = H2; break; }
		H = H2;
	}
	return H;
}

double Placer::compact(const vector<int>& pieces, vector<Vec3>& t, LPBackend& lp, int max_iter) {
	int n = pieces.size();
	auto height = [&]() {
		double h=0;
		for(int i=0; i<n; i++) h = max(h, t[i].z + inst.pieces[pieces[i]].bbox.extent().z);
		return h;
	};

	//region de confianza: la mitad de la menor dimension de las piezas
	double delta = numeric_limits<double>::max();
	for(int i=0; i<n; i++){
		Vec3 e = inst.pieces[pieces[i]].bbox.extent();
		delta = min(delta, 0.5*min(min(e.x, e.y), e.z));
	}

	double H = height();
	vector<PlacedPart> placed;
	for(int it=0; it<max_iter; it++){
		lp.reset(3*n+1);
		int h = 3*n;
		lp.set_bounds(h, 0.0, numeric_limits<double>::infinity());
		lp.set_cost(h, 1.0);
		for(int i=0; i<n; i++){
			Vec3 e = inst.pieces[pieces[i]].bbox.extent();
			double up[3] = { max(0.0, inst.W-e.x), max(0.0, inst.D-e.y), numeric_limits<double>::infinity() };
			for(int c=0; c<3; c++)
				lp.set_bounds(3*i+c, max(0.0, t[i][c]-delta), min(up[c], t[i][c]+delta));
			//empuja las demas piezas hacia abajo (desempate)
			lp.set_cost(3*i+2, 1e-3/n);
			//z_i + e_z <= H
			lp.add_row({ {3*i+2, 1.0}, {h, -1.0} }, -numeric_limits<double>::infinity(), -e.z);
		}

		//pares de partes cercanas
		for(int i=0; i<n; i++)
			for(int j=i+1; j<n; j++){
				for(int a : piece_parts[pieces[i]])
					for(int b : piece_parts[pieces[j]]){
						//rel = t_i - t_j; solapan si rel esta en el interior de NFP(a,b)
						const Box3& ab = parts[a]->get_bbox();
						const Box3& bb = parts[b]->get_bbox();
						Vec3 rel = t[i] - t[j];
						bool near=true;
						for(int c=0; c<3 && near; c++)
							near = rel[c] > bb.lo[c]-ab.hi[c] - 2*delta - tol && rel[c] < bb.hi[c]-ab.lo[c] + 2*delta + tol;
						if(!near) continue;

						const ConvexPolytope& N = nfp(a, b);
						//direccion deseada de rel: la pieza mas alta baja respecto a la otra
						Vec3 g(0, 0, (t[i].z >= t[j].z)? -1.0 : 1.0);
						int best=-1, worst=-1;
						double best_step=-1, best_g=0, worst_s=-numeric_limits<double>::max();
						const vector<Plane>& F = N.get_facets();
						for(int f=0; f<(int)F.size(); f++){
							double s = dot(F[f].n, rel) - F[f].d;
							if(s > worst_s) { worst_s=s; worst=f; }
							if(s < -tol) continue;
							double ng = dot(F[f].n, g);
							double step = (ng >= -1e-12)? numeric_limits<double>::max() : max(0.0,s)/(-ng);
							if(step > best_step || (step==best_step && ng > best_g)) { best_step=step; best_g=ng; best=f; }
						}
						if(best<0) best=worst;
						const Vec3& nn = F[best].n;
						//n.(t_i - t_j) >= d
						lp.add_row({ {3*i, nn.x}, {3*i+1, nn.y}, {3*i+2, nn.z}, {3*j, -nn.x}, {3*j+1, -nn.y}, {3*j+2, -nn.z} },
								F[best].d, numeric_limits<double>::infinity());
					}
			}

		if(!lp.minimize()) break;
		vector<double> x = lp.solution();
		vector<Vec3> t2(n);
		for(int i=0; i<n; i++) t2[i] = Vec3(x[3*i], x[3*i+1], max(0.0, x[3*i+2]));

		//se acepta solo si es factible (tolerancias del LP)
		bool ok=true;
		for(int i=0; i<n && ok; i++){
			place_all(pieces, t2, i, placed);
			ok = feasible(pieces[i], t2[i], placed);
		}
		if(!ok) break;
		t = t2;
		double H2 = height();
		if(H2 >= H - 1e-6*H) { H = H2; break; }
		H = H2;
	}
	return H;
}

double Placer::overlap(const vector<int>& pieces, const vector<Vec3>& t, vector< pair<int,int> >* pairs) const {
	int n = pieces.size();
	vector< vector<ConvexPolytope> > P(n);
	vector<Box3> box(n);
	for(int i=0; i<n; i++)
		for(int a : piece_parts[pieces[i]]){
			P[i].push_back(parts[a]->translated(t[i]));
			box[i].add(P[i].back().get_bbox());
		}
	double total=0;
	if(pairs) pairs->clear();
	for(int i=0; i<n; i++)
		for(int j=i+1; j<n; j++){
			if(!box[i].overlaps(box[j], eps)) continue;
			double d=0;
			for(const ConvexPolytope& A : P[i])
				for(const ConvexPolytope& B : P[j])
					if(A.get_bbox().overlaps(B.get_bbox(), eps)) d = max(d, penetration_depth(A, B));
			if(d > eps) { total += d; if(pairs) pairs->push_back(make_pair(i,j)); }
		}
	return total;
}

bool Placer::reinsert(const vector<int>& pieces, vector<Vec3>& t, int i, double H) {
	vector<PlacedPart> others;
	place_all(pieces, t, i, others);
	double top=0;
	for(size_t k=0; k<pieces.size(); k++)
		if((int)k!=i) top = max(top, t[k].z + inst.pieces[pieces[k]].bbox.extent().z);
	vector<Vec3> cand;
	candidates(pieces[i], others, min(top, H), cand, H);
	if(cand.empty()) return false;
	Vec3 bc = cand[0];
	for(const Vec3& v : cand) if(v.z < bc.z) bc = v;
	t[i] = bc;
	return true;
}

bool Placer::separate(const vector<int>& pieces, vector<Vec3>& t, double H, LPBackend& lp, int max_iter,
		map< pair<int,int>, double >* weights) {
	int n = pieces.size();
	double delta = numeric_limits<double>::max();
	for(int i=0; i<n; i++){
		Vec3 e = inst.pieces[pieces[i]].bbox.extent();
		delta = min(delta, 0.5*min(min(e.x, e.y), e.z));
	}

	map< pair<int,int>, double > wl;         //pesos de los pares de piezas (busqueda local guiada)
	map< pair<int,int>, double >& w = (weights)? *weights : wl;
	map< pair<int,int>, int > rot;           //rotacion de la faceta elegida por par de piezas solapadas
	vector< pair<int,int> > ov;
	double best = overlap(pieces, t, &ov);
	vector<Vec3> best_t = t;
	int stall = 0;
	vector<PlacedPart> others;
	vector<Vec3> cand;
	for(int it=0; it<max_iter && best > eps; it++){
		set< pair<int,int> > ovs(ov.begin(), ov.end());

		//pares de partes cercanas: (pieza i, pieza j, faceta)
		struct Row { int i, j; Vec3 n; double d; };
		vector<Row> rows;
		vector< pair<double,int> > fs;
		for(int i=0; i<n; i++)
			for(int j=i+1; j<n; j++)
				for(int a : piece_parts[pieces[i]])
					for(int b : piece_parts[pieces[j]]){
						const Box3& ab = parts[a]->get_bbox();
						const Box3& bb = parts[b]->get_bbox();
						Vec3 rel = t[i] - t[j];
						bool near=true;
						for(int c=0; c<3 && near; c++)
							near = rel[c] > bb.lo[c]-ab.hi[c] - 2*delta - tol && rel[c] < bb.hi[c]-ab.lo[c] + 2*delta + tol;
						if(!near) continue;
						//facetas ordenadas por holgura (la primera es la mas facil de satisfacer); si el par de piezas
						//sigue solapado se rota entre las 3 primeras
						const vector<Plane>& F = nfp(a, b).get_facets();
						fs.clear();
						for(int f=0; f<(int)F.size(); f++) fs.push_back(make_pair(-(dot(F[f].n, rel) - F[f].d), f));
						int k = 0;
						if(ovs.count(make_pair(i,j)) && fs.size()>1){
							int r = rot[make_pair(i,j)];
							k = min(r % 3, (int) fs.size()-1);
						}
						nth_element(fs.begin(), fs.begin()+k, fs.end());
						int bf = fs[k].second;
						rows.push_back(Row{i, j, F[bf].n, F[bf].d});
					}
		for(const auto& pr : ov) rot[pr]++;

		int m = rows.size();
		lp.reset(3*n + m);
		for(int i=0; i<n; i++){
			Vec3 e = inst.pieces[pieces[i]].bbox.extent();
			double up[3] = { max(0.0, inst.W-e.x), max(0.0, inst.D-e.y), max(0.0, H-e.z) };
			for(int c=0; c<3; c++){
				double lo = max(0.0, t[i][c]-delta), hi = min(up[c], t[i][c]+delta);
				if(lo > hi) lo = hi;
				lp.set_bounds(3*i+c, lo, hi);
			}
			lp.set_cost(3*i+2, 1e-4/n);   //gravedad (desempate)
		}
		for(int k=0; k<m; k++){
			const Row& r = rows[k];
			int s = 3*n + k;
			lp.set_bounds(s, 0.0, numeric_limits<double>::infinity());
			auto it_w = w.find(make_pair(r.i, r.j));
			lp.set_cost(s, 1.0 + ((it_w==w.end())? 0.0 : it_w->second));
			//n.(t_i - t_j) + s >= d
			lp.add_row({ {3*r.i, r.n.x}, {3*r.i+1, r.n.y}, {3*r.i+2, r.n.z},
						 {3*r.j, -r.n.x}, {3*r.j+1, -r.n.y}, {3*r.j+2, -r.n.z}, {s, 1.0} },
					r.d, numeric_limits<double>::infinity());
		}
		if(!lp.minimize()) break;
		vector<double> x = lp.solution();
		for(int i=0; i<n; i++){
			Vec3 e = inst.pieces[pieces[i]].bbox.extent();
			t[i] = Vec3(min(max(x[3*i], 0.0), max(0.0, inst.W-e.x)),
					    min(max(x[3*i+1], 0.0), max(0.0, inst.D-e.y)),
					    min(max(x[3*i+2], 0.0), max(0.0, H-e.z)));
		}

		double o = overlap(pieces, t, &ov);
		if(o < best - 1e-9*max(1.0,best)) { best = o; best_t = t; stall = 0; }
		else stall++;
		//busqueda local guiada: los pares que siguen solapados pesan mas
		for(const auto& pr : ov) w[pr] += 1.0;

		//estancamiento: se reinserta la pieza mas pequena de un par solapado en el espacio libre (altura H)
		if(stall >= 3 && !ov.empty()){
			const auto& pr = ov[it % ov.size()];
			int i = (inst.pieces[pieces[pr.first]].volume <= inst.pieces[pieces[pr.second]].volume)? pr.first : pr.second;
			place_all(pieces, t, i, others);
			double top=0;
			for(int k=0; k<n; k++) if(k!=i) top = max(top, t[k].z + inst.pieces[pieces[k]].bbox.extent().z);
			cand.clear();
			candidates(pieces[i], others, min(top, H), cand, H);
			if(!cand.empty()){
				Vec3 bc = cand[0];
				for(const Vec3& v : cand) if(v.z < bc.z) bc = v;
				t[i] = bc;
				o = overlap(pieces, t, &ov);
				if(o < best - 1e-9*max(1.0,best)) { best = o; best_t = t; }
				stall = 0;
			}
		}
		if(stall >= 10) break;
	}
	t = best_t;
	return best <= eps;
}

} /* namespace plp */

