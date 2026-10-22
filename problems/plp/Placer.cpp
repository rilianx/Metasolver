/*
 * Placer.cpp
 */

#include <algorithm>
#include "Placer.h"
#include "LPBackend.h"

using namespace std;

namespace plp {

Placer::Placer(const Instance& inst, int grid, double slide, double eps) :
		inst(inst), grid(max(grid,2)), slide(slide), eps(eps) {
	double scale = max(max(inst.W, inst.D), inst.height_lower_bound());
	tol = 1e-9*scale;
	for(const Piece& P : inst.pieces){
		piece_parts.push_back(vector<int>());
		for(const ConvexPolytope& C : P.parts){
			piece_parts.back().push_back(parts.size());
			parts.push_back(&C);
		}
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
	for(int a : piece_parts[p]){
		ConvexPolytope A = parts[a]->translated(t);
		for(const PlacedPart& B : placed){
			if(!A.get_bbox().overlaps(B.bbox, eps)) continue;
			if(penetration_depth(A, parts[B.part]->translated(B.t), eps) > eps) return false;
		}
	}
	return true;
}

Vec3 Placer::descend(int p, Vec3 t, const Vec3& c, const vector<PlacedPart>& placed) {
	Vec3 e = inst.pieces[p].bbox.extent();
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

		for(int a : piece_parts[p]){
			const Box3& ab = parts[a]->get_bbox();
			for(const PlacedPart& B : placed){
				//caja de las traslaciones t para las que A y B pueden solaparse
				Box3 nb;
				nb.lo = B.bbox.lo - ab.hi;
				nb.hi = B.bbox.hi - ab.lo;
				if(!nb.overlaps(R, -tol)) continue;

				const ConvexPolytope& N = nfp(a, B.part);
				Vec3 rel = t - B.t;
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
				//n.(t - tB) >= d  <=>  -n.t <= -d - n.tB
				lp.add(-F[best].n, -F[best].d - dot(F[best].n, B.t));
			}
		}

		Vec3 t2 = lp.minimize(c, t);
		if(dot(c, t2) >= dot(c, t) - tol) break;
		t = t2;
	}
	return t;
}

void Placer::candidates(int p, const vector<PlacedPart>& placed, double top, vector<Vec3>& out) {
	Vec3 e = inst.pieces[p].bbox.extent();
	if(e.x > inst.W + eps || e.y > inst.D + eps) return;
	double X = max(0.0, inst.W - e.x), Y = max(0.0, inst.D - e.y);

	for(int i=0; i<grid; i++)
		for(int j=0; j<grid; j++){
			Vec3 t0(X*i/(grid-1), Y*j/(grid-1), top);
			//gravedad y deslizamiento hacia las paredes mas cercanas
			Vec3 c((t0.x <= X/2)? slide : -slide, (t0.y <= Y/2)? slide : -slide, 1.0);
			Vec3 t = descend(p, t0, c, placed);

			//pequenas violaciones numericas de las cotas del contenedor
			t.x = min(max(t.x, 0.0), X);
			t.y = min(max(t.y, 0.0), Y);
			t.z = max(t.z, 0.0);

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

} /* namespace plp */

