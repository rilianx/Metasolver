/*
 * ConvexPolytope.cpp
 */

#include <map>
#include <tuple>
#include <cmath>
#include <algorithm>
#include <random>
#include "ConvexPolytope.h"
#ifdef PLP_WITH_QHULL
extern "C" {
#include <libqhull_r/qhull_ra.h>
}
#endif

using namespace std;

namespace plp {

namespace {

struct HullFace {
	int v[3];
	Vec3 n;
	double d;
	bool alive;
};

HullFace make_face(const vector<Vec3>& p, int a, int b, int c) {
	HullFace f;
	f.v[0]=a; f.v[1]=b; f.v[2]=c;
	Vec3 n = cross(p[b]-p[a], p[c]-p[a]);
	f.n = n / n.norm();
	f.d = dot(f.n, p[a]);
	f.alive = true;
	return f;
}

double dist_to_line(const Vec3& p, const Vec3& a, const Vec3& b) {
	return cross(p-a, b-a).norm() / (b-a).norm();
}

void add_direction(vector<Vec3>& dirs, const Vec3& d, double tol) {
	for(const Vec3& e : dirs)
		if(cross(e,d).norm() <= tol) return;
	dirs.push_back(d);
}

}

#ifdef PLP_WITH_QHULL
/** caras (triangulos) de la envolvente con Qhull (opciones Qt: triangulada, con fusion de caras casi coplanares) */
static bool qhull_faces(const vector<Vec3>& p, vector<HullFace>& faces) {
	vector<coordT> pts(3*p.size());
	for(size_t i=0; i<p.size(); i++) { pts[3*i]=p[i].x; pts[3*i+1]=p[i].y; pts[3*i+2]=p[i].z; }
	qhT qh_qh;
	qhT* qh = &qh_qh;
	QHULL_LIB_CHECK
	qh_zero(qh, NULL);
	char opts[] = "qhull Qt";
	int exitcode = qh_new_qhull(qh, 3, p.size(), pts.data(), False, opts, NULL, NULL);
	if(!exitcode){
		facetT* facet;
		FORALLfacets {
			vertexT *vertex, **vertexp;
			int k=0;
			HullFace f;
			FOREACHvertex_(facet->vertices) { if(k<3) f.v[k] = qh_pointid(qh, vertex->point); k++; }
			if(k!=3) continue;
			f.n = Vec3(facet->normal[0], facet->normal[1], facet->normal[2]);
			f.d = -facet->offset;
			f.alive = true;
			//orientacion de los vertices segun la normal
			if(dot(cross(p[f.v[1]]-p[f.v[0]], p[f.v[2]]-p[f.v[0]]), f.n) < 0) swap(f.v[1], f.v[2]);
			faces.push_back(f);
		}
	}
	qh_freeqhull(qh, !qh_ALL);
	int curlong, totlong;
	qh_memfreeshort(qh, &curlong, &totlong);
	return !exitcode && !faces.empty();
}
#endif

ConvexPolytope ConvexPolytope::hull(const vector<Vec3>& points, double eps, bool edges) {
	if(points.size() < 4) throw runtime_error("hull: se requieren al menos 4 puntos");

	//muchos puntos (p. ej., NFP de partes con muchos vertices): se descartan los puntos interiores a la envolvente
	//de una muestra (los extremos en 26 direcciones y puntos al azar); no pueden ser vertices de la envolvente
	//(sin Qhull; con Qhull los conjuntos grandes se resuelven directamente)
#ifndef PLP_WITH_QHULL
	if(points.size() > 4000){
		Box3 bb0;
		for(const Vec3& q : points) bb0.add(q);
		double tol0 = eps * max(bb0.extent().norm(), 1e-300);
		vector<Vec3> sample;
		for(int dx=-1; dx<=1; dx++) for(int dy=-1; dy<=1; dy++) for(int dz=-1; dz<=1; dz++){
			if(!dx && !dy && !dz) continue;
			Vec3 d(dx, dy, dz);
			int bi=0;
			for(int i=1; i<(int)points.size(); i++) if(dot(d, points[i]) > dot(d, points[bi])) bi=i;
			sample.push_back(points[bi]);
		}
		mt19937 rng0(54321);
		uniform_int_distribution<int> u(0, points.size()-1);
		for(int k=0; k<1500; k++) sample.push_back(points[u(rng0)]);
		try{
			ConvexPolytope S = hull(sample, eps, false);
			vector<Vec3> keep = S.vertices;
			for(const Vec3& q : points){
				bool out=false;
				for(const Plane& f : S.facets) if(dot(f.n, q) - f.d > tol0) { out=true; break; }
				if(out) keep.push_back(q);
			}
			if(keep.size() < points.size()) return hull(keep, eps, edges);
		}catch(exception&){ }
	}
#endif

	Box3 bb;
	for(const Vec3& p : points) bb.add(p);
	double tol = eps * max(bb.extent().norm(), 1e-300);

	const vector<Vec3>& p = points;
	vector<HullFace> faces;
#ifdef PLP_WITH_QHULL
	bool done = points.size() >= 500 && qhull_faces(points, faces);
#else
	bool done = false;
#endif
	if(!done){
	//tetraedro inicial
	int i0=0;
	for(int i=1; i<(int)p.size(); i++) if(p[i].x < p[i0].x) i0=i;
	int i1=-1; double best=tol;
	for(int i=0; i<(int)p.size(); i++) { double d=(p[i]-p[i0]).norm(); if(d>best) {best=d; i1=i;} }
	if(i1<0) throw runtime_error("hull: puntos coincidentes");
	int i2=-1; best=tol;
	for(int i=0; i<(int)p.size(); i++) { double d=dist_to_line(p[i],p[i0],p[i1]); if(d>best) {best=d; i2=i;} }
	if(i2<0) throw runtime_error("hull: puntos colineales");
	HullFace base = make_face(p, i0, i1, i2);
	int i3=-1; best=tol;
	for(int i=0; i<(int)p.size(); i++) { double d=fabs(dot(base.n,p[i])-base.d); if(d>best) {best=d; i3=i;} }
	if(i3<0) throw runtime_error("hull: puntos coplanares");

	if(dot(base.n, p[i3]) - base.d > 0) swap(i1,i2);
	faces.push_back(make_face(p, i0, i1, i2));
	faces.push_back(make_face(p, i0, i3, i1));
	faces.push_back(make_face(p, i1, i3, i2));
	faces.push_back(make_face(p, i2, i3, i0));

	//insercion incremental (en orden aleatorio, con semilla fija)
	vector<int> order(p.size());
	for(int i=0; i<(int)p.size(); i++) order[i]=i;
	mt19937 rng(12345);
	shuffle(order.begin(), order.end(), rng);
	for(int i : order){
		if(i==i0 || i==i1 || i==i2 || i==i3) continue;
		map<pair<int,int>, bool> vis_edges;
		bool visible=false;
		for(HullFace& f : faces){
			if(!f.alive || dot(f.n,p[i]) - f.d <= tol) continue;
			visible=true;
			f.alive=false;
			for(int k=0;k<3;k++) vis_edges[make_pair(f.v[k], f.v[(k+1)%3])]=true;
		}
		if(!visible) continue;

		//aristas del horizonte: su gemela pertenece a una cara no visible
		faces.erase(remove_if(faces.begin(), faces.end(), [](const HullFace& f){ return !f.alive; }), faces.end());
		for(auto& e : vis_edges){
			if(vis_edges.count(make_pair(e.first.second, e.first.first))) continue;
			faces.push_back(make_face(p, e.first.first, e.first.second, i));
		}
	}
	}

	ConvexPolytope P;

	//vertices utilizados
	map<int,int> vidx;
	for(const HullFace& f : faces){
		if(!f.alive) continue;
		for(int k=0;k<3;k++)
			if(!vidx.count(f.v[k])) { vidx[f.v[k]] = P.vertices.size(); P.vertices.push_back(p[f.v[k]]); }
	}

	//volumen
	Vec3 c;
	for(const Vec3& v : P.vertices) c += v;
	c = c / P.vertices.size();
	for(const HullFace& f : faces)
		if(f.alive) P.volume += dot(p[f.v[0]]-c, cross(p[f.v[1]]-c, p[f.v[2]]-c)) / 6.0;

	//fusion de facetas coplanares: caras casi coplanares (p. ej., NFP de partes con muchas caras pequenas) se
	//fusionan en el plano mas externo (el politopo queda apenas mas grande, del lado seguro); se agrupan por la normal
	//cuantizada (tabla hash), y una cara se compara tambien con los planos de las celdas vecinas
	vector<int> plane_of(faces.size(), -1);
	{
		const double q = 1e-6;
		map< tuple<long long,long long,long long>, vector<int> > cell;
		double dtol = max(tol, 1e-6*bb.extent().norm());
		for(int j=0; j<(int)faces.size(); j++){
			const HullFace& f=faces[j];
			if(!f.alive) continue;
			long long cx=llround(f.n.x/q), cy=llround(f.n.y/q), cz=llround(f.n.z/q);
			for(long long dx=-1; dx<=1 && plane_of[j]<0; dx++)
				for(long long dy=-1; dy<=1 && plane_of[j]<0; dy++)
					for(long long dz=-1; dz<=1 && plane_of[j]<0; dz++){
						auto it = cell.find(make_tuple(cx+dx, cy+dy, cz+dz));
						if(it==cell.end()) continue;
						for(int k : it->second)
							if((P.facets[k].n - f.n).norm() <= q && fabs(P.facets[k].d - f.d) <= dtol){ plane_of[j]=k; break; }
					}
			if(plane_of[j]<0){
				plane_of[j]=P.facets.size();
				P.facets.push_back(Plane(f.n, f.d));
				cell[make_tuple(cx,cy,cz)].push_back(plane_of[j]);
			}
			Plane& pl = P.facets[plane_of[j]];
			for(int k=0; k<3; k++) pl.d = max(pl.d, dot(pl.n, p[f.v[k]]));
		}
	}

	//aristas entre facetas distintas
	if(edges){
	map<pair<int,int>, int> edge_face;
	for(int j=0; j<(int)faces.size(); j++){
		if(!faces[j].alive) continue;
		for(int k=0;k<3;k++) edge_face[make_pair(faces[j].v[k], faces[j].v[(k+1)%3])] = j;
	}
	for(auto& e : edge_face){
		int a=e.first.first, b=e.first.second;
		if(a > b) continue;
		auto twin = edge_face.find(make_pair(b,a));
		if(twin!=edge_face.end() && plane_of[twin->second]==plane_of[e.second]) continue;
		Vec3 d = p[b]-p[a];
		add_direction(P.edge_dirs, d/d.norm(), 1e-9);
	}
	}

	P.update_bbox();
	return P;
}

void ConvexPolytope::update_bbox() {
	bbox = Box3();
	for(const Vec3& v : vertices) bbox.add(v);
}

ConvexPolytope ConvexPolytope::transformed(const Mat3& R, const Vec3& t) const {
	ConvexPolytope P(*this);
	for(Vec3& v : P.vertices) v = R*v + t;
	for(Plane& f : P.facets) { f.n = R*f.n; f.d = f.d + dot(f.n, t); }
	for(Vec3& e : P.edge_dirs) e = R*e;
	P.update_bbox();
	return P;
}

ConvexPolytope ConvexPolytope::translated(const Vec3& t) const {
	ConvexPolytope P(*this);
	for(Vec3& v : P.vertices) v = v + t;
	for(Plane& f : P.facets) f.d = f.d + dot(f.n, t);
	P.bbox.lo = bbox.lo + t;
	P.bbox.hi = bbox.hi + t;
	return P;
}

bool ConvexPolytope::contains(const Vec3& p, double eps) const {
	for(const Plane& f : facets)
		if(dot(f.n,p) > f.d + eps) return false;
	return true;
}

namespace {

inline void project(const vector<Vec3>& V, const Vec3& a, double& lo, double& hi) {
	lo = hi = dot(V[0], a);
	for(size_t i=1; i<V.size(); i++){
		double x = dot(V[i], a);
		if(x<lo) lo=x; else if(x>hi) hi=x;
	}
}

}

double penetration_depth(const ConvexPolytope& A, const ConvexPolytope& B, double stop, Vec3* axis) {
	double best = numeric_limits<double>::max();
	Vec3 best_axis;

	auto test = [&](const Vec3& a) -> bool {
		double alo, ahi, blo, bhi;
		project(A.get_vertices(), a, alo, ahi);
		project(B.get_vertices(), a, blo, bhi);
		double ov = min(ahi-blo, bhi-alo);
		if(ov < best) { best=ov; best_axis=a; }
		return best <= stop;
	};

	bool done=false;
	for(const Plane& f : A.get_facets()) if((done=test(f.n))) break;
	if(!done) for(const Plane& f : B.get_facets()) if((done=test(f.n))) break;
	if(!done)
		for(const Vec3& ea : A.get_edge_dirs()){
			for(const Vec3& eb : B.get_edge_dirs()){
				Vec3 a = cross(ea, eb);
				double nn = a.norm();
				if(nn <= 1e-9) continue;
				if((done=test(a/nn))) break;
			}
			if(done) break;
		}

	if(axis) *axis = best_axis;
	return best;
}

} /* namespace plp */
